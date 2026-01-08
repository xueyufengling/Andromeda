#include <andromeda/media/codec.h>

#include <string.h>

using namespace andromeda::media;
using namespace andromeda::common;

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
		LogError("codec open input file ", file, " failed. error code: ", ret);
		goto FAILED;
	}
	if((ret = avformat_find_stream_info(fmt_context, nullptr)) < 0)
	{
		LogError("codec find stream info of ", file, " failed. error code: ", ret);
		goto FAILED;
	}
	return fmt_context;
	FAILED:
	close_file_context(fmt_context);
	return nullptr;
}

void andromeda::media::init_fmt_context_from_codec_context(AVFormatContext* fmt_context, AVCodecContext* codec_context, size_t stream_idx)
{
	int ret = 0;
	const AVCodec* codec = codec_context->codec;
	if((ret = avcodec_parameters_from_context((*(fmt_context->streams + stream_idx))->codecpar, codec_context)) < 0)
		LogError("copy parameters from codec context at stream idx ", stream_idx, " failed. error code: ", ret);
	switch(codec_context->codec_type)
	{
	case AVMediaType::AVMEDIA_TYPE_AUDIO: //如果该codec上下文为音频，则设置为文件的音频输出编码器
		fmt_context->audio_codec_id = codec->id;
		break;
	case AVMediaType::AVMEDIA_TYPE_VIDEO:
		fmt_context->video_codec_id = codec->id;
		break;
	default:
		break;
	}
}

AVFormatContext* andromeda::media::alloc_output_file_context(const char* file, AVCodecContext** codec_contexts, size_t context_num)
{
	AVFormatContext* fmt_context = alloc_output_file_context(file);
	for(size_t idx = 0; idx < context_num; ++idx)
	{
		init_fmt_context_from_codec_context(fmt_context, codec_contexts[idx], idx);
	}
	fmt_context->oformat = av_guess_format(nullptr, file, nullptr);
	return fmt_context;
}

void andromeda::media::close_file_context(AVFormatContext* fmt_context)
{
	if(fmt_context)
		avformat_close_input(&fmt_context);
}

bool andromeda::media::alloc_send_recv_buffer(AVPacket** packet, AVFrame** frame)
{
	*frame = av_frame_alloc();
	if(!*frame)
	{
		LogError("codec allocate encoder frame failed");
		return false;
	}
	*packet = av_packet_alloc();
	if(!*packet)
	{
		LogError("codec allocate encoder packet failed");
		return false;
	}
	return true;
}

AVPacket* andromeda::media::read_packet(AVFormatContext* fmt_context)
{
	AVPacket* decoder_packet = nullptr;
	int ret = 0;
	if((ret = av_read_frame(fmt_context, decoder_packet)) < 0)
	{
		LogError("AVFormatContext read packet failed. error code: ", ret);
		return nullptr;
	}
	return decoder_packet;
}

bool encoder::initialize_encoder(AVCodecID codec_id, codec_init init_proc)
{
	encoded_frame_count = 0;
	encoder_codec = (AVCodec*)avcodec_find_encoder(codec_id);
	if(!encoder_codec)
	{
		LogError("codec find encoder ", codec_id, " failed");
		goto FAILED;
	}
	encoder_context = avcodec_alloc_context3(encoder_codec);
	if(!encoder_context)
	{
		LogError("codec ", codec_id, " allocate encoder context failed");
		goto FAILED;
	}
	if(!raw_frame)
	{
		LogError("codec ", codec_id, " allocate encoder frame failed");
		goto FAILED;
	}
	encoded_packet = av_packet_alloc();
	if(!encoded_packet)
	{
		LogError("codec ", codec_id, " allocate encoder packet failed");
		goto FAILED;
	}
	encoder_context->codec_id = codec_id;
	encoder_id = codec_id;
	if(!init_proc(encoder_context))
		goto FAILED;
	if(!allocate_encode_buffer())
		goto FAILED;
	return true;
	FAILED:
	terminate_encoder();
	return false;
}

bool encoder::allocate_encode_buffer()
{
	return alloc_send_recv_buffer(&encoded_packet, &raw_frame);;
}

void encoder::terminate_encoder()
{
	if(encoded_packet)
	{
		av_packet_free(&encoded_packet);
		encoded_packet = nullptr;
	}
	if(raw_frame)
	{
		av_frame_free(&raw_frame);
		raw_frame = nullptr;
	}
	if(encoder_context)
	{
		avcodec_free_context(&encoder_context);
		encoder_context = nullptr;
	}
	encoder_id = AVCodecID::AV_CODEC_ID_NONE;
}

bool decoder::initialize_decoder(AVCodecID codec_id, codec_init init_proc)
{
	decoder_id = codec_id;
	decoder_codec = (AVCodec*)avcodec_find_decoder(codec_id);
	if(!decoder_codec)
	{
		LogError("codec find decoder ", codec_id, " failed");
		goto FAILED;
	}
	decoder_context = avcodec_alloc_context3(decoder_codec);
	if(!decoder_context)
	{
		LogError("decoder ", codec_id, " allocate decoder context failed");
		goto FAILED;
	}
	decoder_parser = av_parser_init(codec_id);
	if(!decoder_parser)
	{
		LogError("decoder ", codec_id, " init parser failed");
		goto FAILED;
	}
	if(!allocate_decode_buffer())
		goto FAILED;
	decoder_context->codec_id = codec_id;
	if(init_proc && !init_proc(decoder_context))
	{
		LogError("decoder ", codec_id, " execute init_proc failed");
		goto FAILED;
	}
	return true;
	FAILED:
	terminate_decoder();
	return false;
}

bool decoder::initialize_decoder(AVCodecParameters* codecpar, codec_init init_proc)
{
	int ret = 0;
	initialize_decoder(codecpar->codec_id);
	if((ret = avcodec_parameters_to_context(decoder_context, codecpar)) < 0)
	{
		LogError("decoder ", decoder_id, " copy params failed. error code: ", ret);
		goto FAILED;
	}
	if(init_proc && !init_proc(decoder_context))
	{
		LogError("decoder ", decoder_id, " execute init_proc failed");
		goto FAILED;
	}
	return true;
	FAILED:
	terminate_decoder();
	return false;
}

bool decoder::open_decoder()
{
	int ret = 0;
	if((ret = avcodec_open2(decoder_context, decoder_codec, nullptr)) < 0) //打开流所使用的codec
	{
		LogError("open codec ", decoder_id, " failed. error code: ", ret);
		return false;
	}
	return true;
}

bool decoder::allocate_decode_buffer()
{
	return alloc_send_recv_buffer(&raw_packet, &decoded_frame);
}

void decoder::terminate_decoder()
{
	if(decoded_frame)
	{
		av_frame_free(&decoded_frame);
		decoded_frame = nullptr;
	}
	if(raw_packet)
	{
		av_packet_free(&raw_packet);
		raw_packet = nullptr;
	}
	if(decoder_context)
	{
		avcodec_free_context(&decoder_context);
		decoder_context = nullptr;
	}
	if(decoder_parser)
	{
		av_parser_close(decoder_parser);
		decoder_parser = nullptr;
	}
	decoder_codec = nullptr;
	decoder_id = AVCodecID::AV_CODEC_ID_NONE;
}

int decoder::read_packet(AVPacket* packet, void* data, size_t size)
{
	int ret = 0;
	if((ret = av_parser_parse2(decoder_parser, decoder_context, &packet->data, &packet->size, (const unsigned char*)data, size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0)) < 0)
	{
		LogError("decoder read packet failed. error code: ", ret);
	}
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
