#include <andromeda/media/codec.h>

#include <string.h>
#include <andromeda/media/pcm_audio.h>

using namespace andromeda::media;
using namespace andromeda::util;

frame_proc andromeda::media::audio_decoded_frame_proc = [](buffer& data_buffer, AVCodecContext* decoder_context, AVFrame* frame) -> void
		{
			for(int cp = 0; cp < decoder_context->channels; ++cp)
			{
				data_buffer.add(frame->extended_data[cp], (size_t)frame->linesize[cp]);
			}
		};

codec_init andromeda::media::no_extra_codec_init = [](AVCodecContext* decoder_context) -> bool
		{
			return decoder_context;
		};

AVFormatContext* andromeda::media::open_input_file_context(const char* file)
{
	int ret = 0;
	AVFormatContext* fmt_context = nullptr;
	if((ret = avformat_open_input(&fmt_context, file, nullptr, nullptr)) < 0)
	{
		LogError(ret, "codec open input file ", file, " failed. error code: ", ret);
		goto FAILED;
	}
	if((ret = avformat_find_stream_info(fmt_context, nullptr)) < 0)
	{
		LogError(ret, "codec find stream info of ", file, " failed. error code: ", ret);
		goto FAILED;
	}
	return fmt_context;
	FAILED:
	close_file_context(fmt_context);
	return nullptr;
}

AVFormatContext* andromeda::media::open_output_file_context(const char* file)
{
	AVFormatContext* fmt_context = avformat_alloc_context();
	if(!fmt_context)
	{
		LogError("codec allocate output format context for file ", file, " failed");
		return nullptr;
	}
	strcpy(fmt_context->filename, file);
	return fmt_context;
}

void andromeda::media::close_file_context(AVFormatContext* fmt_context)
{
	if(fmt_context)
		avformat_close_input(&fmt_context);
}

bool encoder::initialize_encoder(AVCodecID id, codec_init init_proc)
{
	int ret = 0;
	encoded_frame_count = 0;
	encoder_codec = avcodec_find_encoder(id);
	if(!encoder_codec)
	{
		LogError("codec find encoder ", id, " failed");
		goto FAILED;
	}
	encoder_context = avcodec_alloc_context3(encoder_codec);
	if(!encoder_context)
	{
		LogError("codec ", id, " allocate encoder context failed");
		goto FAILED;
	}
	encoder_frame = av_frame_alloc();
	if(!encoder_frame)
	{
		LogError(ret, "codec ", id, " allocate encoder frame failed");
		goto FAILED;
	}
	encoder_packet = av_packet_alloc();
	if(!encoder_packet)
	{
		LogError(ret, "codec ", id, " allocate encoder packet failed");
		goto FAILED;
	}
	encoder_context->codec_id = id;
	encoder_id = id;
	if(!init_proc(encoder_context))
		goto FAILED;
	return true;
	FAILED:
	terminate_encoder();
	return false;
}

void encoder::terminate_encoder()
{
	if(encoder_packet)
	{
		av_free_packet(encoder_packet);
		encoder_packet = nullptr;
	}
	if(encoder_frame)
	{
		av_frame_free(&encoder_frame);
		encoder_frame = nullptr;
	}
	if(encoder_context)
	{
		avcodec_free_context(&encoder_context);
		encoder_context = nullptr;
	}
	encoder_id = AVCodecID::AV_CODEC_ID_NONE;
}

bool decoder::initialize_decoder(AVCodecID id, codec_init init_proc)
{
	int ret = 0;
	decoder_codec = avcodec_find_decoder(id); //根据ID查找对应的codec
	if(!decoder_codec)
	{
		LogError("codec find decoder ", id, " failed");
		goto FAILED;
	}
	decoder_context = avcodec_alloc_context3(decoder_codec); //为codec创建一个新的codec上下文
	if(!decoder_context)
	{
		LogError("codec ", id, " allocate decoder context failed");
		goto FAILED;
	}
	if((ret = avcodec_open2(decoder_context, decoder_codec, nullptr)) < 0) //打开codec
	{
		LogError("open codec ", id, " failed. error code: ", ret);
		goto FAILED;
	}
	decoder_context->codec_id = id;
	decoder_id = id;
	if(!init_proc(decoder_context))
		goto FAILED;
	if(!allocate_decode_buffer())
		goto FAILED;
	return true;
	FAILED:
	terminate_decoder();
	return false;
}

bool decoder::initialize_decoder(AVStream* stream, codec_init init_proc)
{
	int ret = 0;
	decoder_context = stream->codec; //codec上下文直接从流中获取
	decoder_id = decoder_context->codec_id;
	decoder_codec = avcodec_find_decoder(decoder_id); //根据流的codec上下文中储存的ID查找codec
	if(!decoder_codec)
	{
		LogError("codec find decoder ", decoder_id, " failed");
		goto FAILED;
	}
	if((ret = avcodec_open2(decoder_context, decoder_codec, nullptr)) < 0) //打开流所使用的codec
	{
		LogError("open codec ", decoder_id, " failed. error code: ", ret);
		goto FAILED;
	}
	if(!init_proc(decoder_context))
		goto FAILED;
	if(!allocate_decode_buffer()) //创建并打开codec成功则分配解码缓冲
		goto FAILED;
	return true;
	FAILED:
	terminate_decoder();
	return false;
}

bool decoder::allocate_decode_buffer()
{
	int ret = 0;
	decoder_frame = av_frame_alloc();
	if(!decoder_frame)
	{
		LogError(ret, "codec ", decoder_id, " allocate decoder frame failed");
		return false;
	}
	return true;
}

void decoder::terminate_decoder()
{
	if(decoder_frame)
	{
		av_frame_free(&decoder_frame);
		decoder_frame = nullptr;
	}
	if(decoder_context)
	{
		avcodec_free_context(&decoder_context);
		decoder_context = nullptr;
	}
	decoder_id = AVCodecID::AV_CODEC_ID_NONE;
}

AVPacket* decoder::read_packet(AVFormatContext* fmt_context)
{
	AVPacket* decoder_packet = nullptr;
	int ret = 0;
	if((ret = av_read_frame(fmt_context, decoder_packet)) < 0)
	{
		LogError(ret, "decoder read packet failed. error code: ", ret);
		return nullptr;
	}
	return decoder_packet;
}

int decoder::decode(buffer& data_buffer, AVPacket* packet, frame_proc proc, bool flush, bool unref_packet)
{
	int ret = 0;
	if((ret = avcodec_send_packet(decoder_context, packet)) < 0)
	{
		if(packet)
		{
			LogError("decoder send the packet failed. error code: ", ret);
		}
		else
		{
			LogError("decoder flush the packet failed. error code: ", ret);
		}
		goto FAILED;
	}
	while(ret >= 0)
	{
		ret = avcodec_receive_frame(decoder_context, decoder_frame);
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) //全部帧读取完毕
			break;
		else if(ret < 0)
		{
			LogError("decoder receive the frame failed");
			goto FAILED;
		}
		else
			proc(data_buffer, decoder_context, decoder_frame);
	}
	if(flush)
		decode(data_buffer, nullptr, proc, false, unref_packet); //如果packet不是nullptr则对解码缓冲区剩余内容解码，并清空缓冲区
	av_packet_unref(packet);
	return ret;
	FAILED:
	avcodec_send_packet(decoder_context, nullptr); //遇到错误则清空缓冲区
	return ret;
}

int media_decoder::open_input_file(const char* file)
{
	fmt_context = open_input_file_context(file);
	if(!fmt_context)
		return 0; //未打开文件
	stream_decoder_codecs.clear();
	stream_decoder_codecs.reserve(stream_num());
	for(size_t idx = 0; idx < stream_num(); ++idx)
	{
		stream_decoder_codecs[idx].initialize_decoder(stream(idx), no_extra_codec_init);
	}
	return stream_num();
}
