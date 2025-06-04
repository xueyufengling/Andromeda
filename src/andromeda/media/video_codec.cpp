#include <andromeda/media/video_codec.h>

#include <iostream>

#include <andromeda/media/raster_image.h>
#include <andromeda/io/files.h>
#include <andromeda/media/codec.h>
#include <andromeda/media/image_factory.h>
#include <andromeda/media/video_factory.h>

using namespace andromeda::media;
using namespace andromeda::io;

VideoCodec::VideoCodec(AVCodecID id)
{
	construct(id);
}

VideoCodec::~VideoCodec()
{
	deconstruct();
}

bool VideoCodec::construct(AVCodecID id)
{
	encoded_frame_count=0;
	is_buf_alloc=false;
	codec_context=nullptr;
	codec=nullptr;
	frame=nullptr;
	packet=nullptr;
	output_file=nullptr;
	properties=new VideoProperties();
	if(!init_codec(id))
		return false;
	packet=av_packet_alloc();
	if(!packet)
	{
		std::cout<<"Allocate video packet failed."<<std::endl;
		return false;
	}
	return true;
}

void VideoCodec::deconstruct()
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
	codec=nullptr;
	codec_context=nullptr;
	frame=nullptr;
	delete properties;
}

bool VideoCodec::init_codec(AVCodecID id)
{
	codec=avcodec_find_encoder(id);
	if(!codec)
	{
		std::cout<<"Find encoder failed."<<std::endl;
		return false;
	}
	codec_context=avcodec_alloc_context3(codec);
	codec_context->codec_id=id;
	codec_context->pix_fmt=AV_PIX_FMT_YUV444P;
	codec_context->width=0;
	codec_context->height=0;
	if(!codec_context)
	{
		std::cout<<"Allocate encoder context failed."<<std::endl;
		return false;
	}
	return true;
}

void VideoCodec::inputFrame(RasterImage& img,int last_frame_count)
{
	if(last_frame_count<1)
		return;
	VideoCodec::loadYUV444PDataFromRasterImageToFrame(img,frame);
	for(;last_frame_count;--last_frame_count)
	{
		startEncode(codec_context,frame,nullptr,packet,&encoded_frame_count)
output_file			->write((const char*)(packet->data),packet->size);
			endEncode(codec_context,frame,nullptr,packet,&encoded_frame_count)
	}
}

void VideoCodec::applySettings(void)
{
	int ret=0;
	if(is_buf_alloc)
	{
		if(codec_context->width==properties->width&&codec_context->height==properties->height)
			goto NO_ALLOC;
		av_frame_free(&frame);
		is_buf_alloc=false;
		frame=nullptr;
	}
	frame=av_frame_alloc();
	if(!frame)
	{
		std::cout<<"Allocate frame failed."<<std::endl;
		return;
	}
	frame->format=codec_context->pix_fmt;
	NO_ALLOC:if(codec_context)
	{
		codec_context->width=properties->width;
		codec_context->height=properties->height;
		codec_context->time_base=(properties->timebase_num==0||properties->timebase_den==0)?(AVRational){properties->frame_rate_den,properties->frame_rate_num}:(AVRational){properties->timebase_num,properties->timebase_den};
		codec_context->framerate=(AVRational){properties->frame_rate_num,properties->frame_rate_den};
		codec_context->bit_rate=properties->bit_rate;
		codec_context->gop_size=properties->gop_size;
		codec_context->max_b_frames=properties->max_b_frames;
	}
	frame->width=properties->width;
	frame->height=properties->height;
	ret=av_frame_get_buffer(frame,0);
	if(ret<0)
		logError(ret,"Get buffer of frame failed.");
	ret=av_frame_make_writable(frame);
	if(ret<0)
		logError(ret,"Make buffer of frame writable failed.");
	is_buf_alloc=true;
	ret=avcodec_open2(codec_context,codec,0);
	if(ret<0)
		logError(ret,"Active encoder context failed.");
}

void VideoCodec::loadYUV444PDataFromRasterImageToFrame(RasterImage& img,AVFrame* frame_)
{
	frame_->format=AV_PIX_FMT_YUV444P;
	convertRGBA32Tou255YUV444P((unsigned char*)(img.getPixelDataPtr()),img.getWidth(),img.getHeight(),0,0,0,(unsigned char*)frame_->data[0],frame_->linesize[0],(unsigned char*)frame_->data[1],frame_->linesize[1],(unsigned char*)(frame_->data[2]),frame_->linesize[2]);
}

void VideoCodec::openOutputBitstreamFile(const char* file)
{
	clearFile(file);
	output_file=new std::ofstream(file,std::ios::app|std::ios::binary);
	encoded_frame_count=0;
}

void VideoCodec::closeOutputBitstreamFile(void)
{
	startEncodeFlush(codec_context,nullptr,packet)
output_file		->write((const char*)(packet->data),packet->size);
		endEncodeFlush(codec_context,nullptr,packet)
		output_file->close();
		delete output_file;
		output_file=nullptr;
		encoded_frame_count=0;
	}

void VideoCodec::inputFrames(VideoFrameInfo* infos,int length)
{
	for(int pos=0;pos<length;++pos)
		inputFrame(*((infos+pos)->img),(*(infos+pos)).last_frame_count);
}

VideoFrameInfo::VideoFrameInfo(RasterImage& image) :
		img(&image), last_frame_count(1)
{
}

VideoFrameInfo::VideoFrameInfo(RasterImage& image,int lfc) :
		img(&image), last_frame_count(lfc)
{
}
