#include <andromeda/media/video_codec.h>

#include <iostream>

#include <andromeda/media/raster_image.h>
#include <andromeda/io/files.h>
#include <andromeda/util/log.h>

using namespace andromeda::media;
using namespace andromeda::io;

video_codec::video_codec(AVCodecID id)
{
	construct(id);
}

video_codec::~video_codec()
{
	deconstruct();
}

bool video_codec::construct(AVCodecID id)
{
	encoded_frame_count = 0;
	is_buf_alloc = false;
	codec_context = nullptr;
	codec = nullptr;
	frame = nullptr;
	packet = nullptr;
	output_file = nullptr;
	video_properties = new properties();
	if(!init_codec(id))
		return false;
	packet = av_packet_alloc();
	if(!packet)
	{
		std::cout << "Allocate video packet failed." << std::endl;
		return false;
	}
	return true;
}

void video_codec::deconstruct()
{
	if(codec_context)
	{
		avcodec_close(codec_context);
		avcodec_free_context(&codec_context);
	}
	if(frame)
		av_frame_free(&frame);
	if(packet)
		av_packet_free(&packet);
	codec = nullptr;
	codec_context = nullptr;
	frame = nullptr;
	delete video_properties;
}

bool video_codec::init_codec(AVCodecID id)
{
	codec = avcodec_find_encoder(id);
	if(!codec)
	{
		std::cout << "Find encoder failed." << std::endl;
		return false;
	}
	codec_context = avcodec_alloc_context3(codec);
	codec_context->codec_id = id;
	codec_context->pix_fmt = AV_PIX_FMT_YUV444P;
	codec_context->width = 0;
	codec_context->height = 0;
	if(!codec_context)
	{
		std::cout << "Allocate encoder context failed." << std::endl;
		return false;
	}
	return true;
}

void video_codec::append_frame(raster_image& img, int last_frame_count)
{
	if(last_frame_count < 1)
		return;
	video_codec::loadYUV444PDataFromRasterImageToFrame(img, frame);
	for(; last_frame_count; --last_frame_count)
	{
		StartEncode(codec_context,frame,nullptr,packet,&encoded_frame_count)
output_file			->write((const char*)(packet->data),packet->size);
			EndEncode(codec_context, frame, nullptr, packet, &encoded_frame_count)
			}
}

void video_codec::applySettings(void)
{
	int ret = 0;
	if(is_buf_alloc)
	{
		if(codec_context->width == video_properties->width && codec_context->height == video_properties->height)
			goto NO_ALLOC;
		av_frame_free(&frame);
		is_buf_alloc = false;
		frame = nullptr;
	}
	frame = av_frame_alloc();
	if(!frame)
	{
		std::cout << "Allocate frame failed." << std::endl;
		return;
	}
	frame->format = codec_context->pix_fmt;
	NO_ALLOC: if(codec_context)
	{
		codec_context->width = video_properties->width;
		codec_context->height = video_properties->height;
		codec_context->time_base = (video_properties->timebase_num == 0 || video_properties->timebase_den == 0) ? (AVRational){video_properties->frame_rate_den, video_properties->frame_rate_num} : (AVRational){video_properties->timebase_num, video_properties->timebase_den};
		codec_context->framerate = (AVRational){video_properties->frame_rate_num, video_properties->frame_rate_den};
		codec_context->bit_rate = video_properties->bit_rate;
		codec_context->gop_size = video_properties->gop_size;
		codec_context->max_b_frames = video_properties->max_b_frames;
	}
	frame->width = video_properties->width;
	frame->height = video_properties->height;
	ret = av_frame_get_buffer(frame, 0);
	if(ret < 0)
		LogError(ret, "Get buffer of frame failed.");
	ret = av_frame_make_writable(frame);
	if(ret < 0)
		LogError(ret, "Make buffer of frame writable failed.");
	is_buf_alloc = true;
	ret = avcodec_open2(codec_context, codec, 0);
	if(ret < 0)
		LogError(ret, "Active encoder context failed.");
}

void video_codec::loadYUV444PDataFromRasterImageToFrame(raster_image& img, AVFrame* frame_)
{
	frame_->format = AV_PIX_FMT_YUV444P;
	convertRGBA32Tou255YUV444P((unsigned char*)(img.get_pixels()), img.get_width(), img.get_height(), 0, 0, 0, (unsigned char*)frame_->data[0], frame_->linesize[0], (unsigned char*)frame_->data[1], frame_->linesize[1], (unsigned char*)(frame_->data[2]), frame_->linesize[2]);
}

void video_codec::openOutputBitstreamFile(const char* file)
{
	clear(file);
	output_file = new std::ofstream(file, std::ios::app | std::ios::binary);
	encoded_frame_count = 0;
}

void video_codec::closeOutputBitstreamFile(void)
{
	StartEncodeFlush(codec_context,nullptr,packet)
output_file		->write((const char*)(packet->data),packet->size);
		EndEncodeFlush(codec_context,nullptr,packet)
		output_file->close();
		delete output_file;
		output_file=nullptr;
		encoded_frame_count=0;
	}

void video_codec::append_frame(frame_info* infos, int length)
{
	for(int pos = 0; pos < length; ++pos)
		append_frame(*((infos + pos)->img), (*(infos + pos)).last_frame_count);
}

void andromeda::media::init_video_stream(AVFormatContext* fc, AVCodecContext* cc, AVCodec* c, int pos)
{
	AVStream* stream = *(fc->streams) + pos;
	stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	stream->codecpar->codec_id = c->id;
	stream->codecpar->width = cc->width;
	stream->codecpar->height = cc->height;
	stream->sample_aspect_ratio = (AVRational){cc->width, cc->height};
	stream->codecpar->bit_rate = cc->bit_rate;
	stream->codecpar->format = AV_PIX_FMT_YUV444P;
	stream->time_base = cc->time_base;
	stream->avg_frame_rate = cc->framerate;
	stream->id = pos;
	stream->index = pos;
	int ret = 0;
	ret = avcodec_parameters_from_context((*(fc->streams) + pos)->codecpar, cc);
	if(ret < 0)
		LogError(ret, "Copy parameters from media encoder context failed.");
}

void andromeda::media::append_stream(AVFormatContext* fc, AVCodec* c, int num)
{
	for(; num; --num)
		avformat_new_stream(fc, c);
}

int andromeda::media::append_stream_to(AVFormatContext* fc, AVCodec* c, int lim)
{
	int num = 0;
	for(; fc->nb_streams < lim; ++num)
		avformat_new_stream(fc, c);
	return num;
}

void andromeda::media::encode_flush(AVCodecContext* cc, AVFormatContext* fc, AVStream* st, AVPacket* pkt, packet_proc p)
{
	avcodec_send_frame(cc, nullptr);
	int ret = 0;
	while(ret >= 0)
	{
		ret = avcodec_receive_packet(cc, pkt);
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if(ret < 0)
		{
			std::cout << "Receive the packet failed." << std::endl;
			break;
		}
		if(st)
		{
			av_packet_rescale_ts(pkt, cc->time_base, st->time_base);
			pkt->stream_index = st->index;
		}
		p(cc, fc, pkt);
		av_packet_unref(pkt);
	}
}

void andromeda::media::encode(AVCodecContext* cc, AVFrame* frm, AVFormatContext* fc, AVStream* st, AVPacket* pkt, long int* count, packet_proc p)
{
	frm->pts = *count;
	if(avcodec_send_frame(cc, frm) < 0)
	{
		std::cout << "Send the frame failed." << std::endl;
		return;
	}
	int ret = 0;
	while(ret >= 0)
	{
		ret = avcodec_receive_packet(cc, pkt);
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if(ret < 0)
		{
			std::cout << "Receive the packet failed." << std::endl;
			break;
		}
		p(cc, fc, pkt);
	}
	++*count;
}

void andromeda::media::decode(AVCodecContext* cc, AVPacket* pkt, AVFormatContext* fc, AVFrame* frm, frame_proc p)
{
	if(avcodec_send_packet(cc, pkt) < 0)
	{
		std::cout << "Send the packet failed." << std::endl;
		return;
	}
	int ret = 0;
	while(ret >= 0)
	{
		ret = avcodec_receive_frame(cc, frm);
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if(ret < 0)
		{
			std::cout << "Receive the frame failed." << std::endl;
			break;
		}
		p(cc, fc, frm);
	}
}

void andromeda::media::decode_flush(AVCodecContext* cc, AVFormatContext* fc, AVFrame* frm, frame_proc p)
{
	avcodec_send_packet(cc, nullptr);
	int ret = 0;
	while(ret >= 0)
	{
		ret = avcodec_receive_frame(cc, frm);
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if(ret < 0)
		{
			std::cout << "Receive the frame failed." << std::endl;
			break;
		}
		p(cc, fc, frm);
	}
}
