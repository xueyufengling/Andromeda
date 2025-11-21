#ifndef ANDROMEDA_MEDIA_CODEC
#define ANDROMEDA_MEDIA_CODEC

#include <functional>
#include "../util/array.h"
#include "../util/log.h"

extern "C"
{
#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>
}

namespace andromeda
{
namespace media
{

typedef std::function<void(void*, AVCodecContext*, AVPacket*)> packet_proc;
typedef std::function<void(andromeda::util::buffer&, AVCodecContext*, AVFrame*)> frame_proc;
typedef std::function<bool(AVCodecContext*)> codec_init;

extern frame_proc audio_decoded_frame_proc;
extern codec_init no_extra_codec_init;

/**
 * @brief 打开一个文件输入上下文
 */
AVFormatContext* open_input_file_context(const char* file);

template<int StreamNum>
inline void _alloc_output_file_context(AVFormatContext* fmt_context)
{
}

template<int StreamNum, typename C, typename ...Cs>
inline void _alloc_output_file_context(AVFormatContext* fmt_context, C codec_context, Cs* ... codec_contexts)
{
	int ret = 0;
	if((ret = avcodec_parameters_from_context((*(fmt_context->streams + (StreamNum - sizeof...(Cs) - 1)))->codecpar, codec_context)) < 0)
		LogError("copy parameters from codec context failed. error code: ", ret);
	_alloc_output_file_context<StreamNum>(codec_contexts...);
}

/**
 * @brief 打开一个文件输出上下文
 */
template<typename ...Cs>
AVFormatContext* alloc_output_file_context(const char* file, Cs* ... codec_contexts)
{
	AVFormatContext* fmt_context = avformat_alloc_context();
	if(!fmt_context)
	{
		LogError("codec allocate output format context for file ", file, " failed");
		return nullptr;
	}
	_alloc_output_file_context<sizeof...(Cs)>(fmt_context, codec_contexts...); //将codec的参数按顺序拷贝进AVFormatContext
	strcpy(fmt_context->filename, file);
	return fmt_context;
}

AVFormatContext* alloc_output_file_context(const char* file, AVCodecContext** codec_contexts, size_t context_num);

/**
 * @brief 关闭一个文件上下文
 */
void close_file_context(AVFormatContext* fmt_context);

//编码
class encoder
{
protected:
	AVCodecID encoder_id = AVCodecID::AV_CODEC_ID_NONE;
	AVCodec* encoder_codec = nullptr;
	AVCodecContext* encoder_context = nullptr;
	AVFrame* encoder_frame = nullptr;
	AVPacket* encoder_packet = nullptr;

	int64_t encoded_frame_count, //已编码的帧的数量，用作时间戳
			packet_count;

public:

	inline AVCodecID get_encoder_id()
	{
		return encoder_id;
	}

	/**
	 * @brief 初始化编码器
	 */
	bool initialize_encoder(AVCodecID id, codec_init init_proc);

	/**
	 * @brief 释放编码器占用资源
	 */
	void terminate_encoder();

};

class decoder
{
protected:

	//解码
	AVCodecID decoder_id = AVCodecID::AV_CODEC_ID_NONE;
	AVCodec* decoder_codec = nullptr;
	AVCodecContext* decoder_context = nullptr;
	AVFrame* decoder_frame = nullptr;

	int64_t encoded_frame_count, //已解码的帧的数量
			packet_count;

	bool allocate_decode_buffer();

public:
	inline AVCodecID get_decoder_id()
	{
		return decoder_id;
	}

	bool initialize_decoder(AVCodecID id, codec_init init_proc);

	bool initialize_decoder(AVStream* stream, codec_init init_proc);

	void terminate_decoder();

	static AVPacket* read_packet(AVFormatContext* fmt_context);

	int decode(andromeda::util::buffer& data_buffer, AVPacket* packet, frame_proc proc, bool flush = false, bool unref_packet = true);

};

/**
 * @brief 文件解码器
 */
class media_decoder
{
protected:
	AVFormatContext* fmt_context = nullptr;
	std::vector<decoder> stream_decoder_codecs;

public:
	inline decoder& operator [](int idx)
	{
		return stream_decoder_codecs[idx];
	}

	/**
	 * @brief 打开一个媒体文件并获取所有流的信息，将各个流的编码器进行初始化并打开准备解码
	 * @return 文件的流数量，打开失败返回0
	 */
	int open_input_file(const char* file);

	/**
	 * @brief 当前输入文件的流的数量
	 */
	inline size_t stream_num()
	{
		return fmt_context->nb_streams;
	}

	inline AVStream* stream(size_t idx)
	{
		return fmt_context->streams[idx];
	}

	inline AVMediaType stream_type(size_t idx)
	{
		return stream(idx)->codec->codec_type;
	}
};

}
}

#define set_property(obj,key,val) obj->properties->key=val

//cc=codec_context*,frm=avframe*,st=avstream*,pkt=avpacket*,frmc=frame pts counter(int*)
#define StartEncode(cc,frm,st,pkt,frmc)\
        {\
            ((AVFrame*)(frm))->pts=*((int*)(frmc));\
            if(avcodec_send_frame(((AVCodecContext*)(cc)),((AVFrame*)(frm)))<0)\
            {\
                std::cout<<"Send the frame failed."<<std::endl;\
            }\
            int ret_enc=0;\
            while(ret_enc>=0)\
            {\
                ret_enc=avcodec_receive_packet(((AVCodecContext*)(cc)),((AVPacket*)(pkt)));\
                if(ret_enc==AVERROR(EAGAIN)||ret_enc==AVERROR_EOF)\
                    break;\
                else if(ret_enc<0)\
                {\
                    std::cout<<"Receive the packet failed."<<std::endl;\
                    break;\
                }\
                ((AVPacket*)(pkt))->pts=((AVFrame*)(frm))->pts;\
                if(((AVStream*)(st))){\
                    av_packet_rescale_ts(((AVPacket*)(pkt)),((AVCodecContext*)(cc))->time_base,((AVStream*)(st))->time_base);\
                    ((AVPacket*)(pkt))->stream_index=((AVStream*)(st))->index;\
                }\
                {

#define EndEncode(cc,frm,st,pkt,frmc)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
            ++*((int*)(frmc));\
        }

#define StartEncodeFlush(cc,st,pkt)\
        {\
            avcodec_send_frame(((AVCodecContext*)(cc)),nullptr);\
            int ret_enc_flu=0;\
            while(ret_enc_flu>=0)\
            {\
                ret_enc_flu=avcodec_receive_packet(((AVCodecContext*)(cc)),((AVPacket*)(pkt)));std::cout<<"cl"<<std::endl;\
                if(ret_enc_flu==AVERROR(EAGAIN)||ret_enc_flu==AVERROR_EOF)\
                    break;\
                else if(ret_enc_flu<0)\
                {\
                    std::cout<<"Receive the packet failed."<<std::endl;\
                    break;\
                }\
                if(((AVStream*)(st)))\
                {\
                    av_packet_rescale_ts(((AVPacket*)(pkt)),((AVCodecContext*)(cc))->time_base,((AVStream*)(st))->time_base);\
                    ((AVPacket*)(pkt))->stream_index=((AVStream*)(st))->index;\
                }\
                {

#define EndEncodeFlush(cc,st,pkt)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
        }

#define StartDecode(cc,pkt,frm)\
        {\
            if(avcodec_send_packet(cc,pkt)<0)\
            {\
                std::cout<<"Send the packet failed."<<std::endl;\
            }\
            int ret_dec=0;\
            while(ret_dec>=0)\
            {\
                ret_dec=avcodec_receive_frame(cc,frm);\
                if(ret_dec==AVERROR(EAGAIN)||ret_dec==AVERROR_EOF)\
                    break;\
                else if(ret_dec<0)\
                {\
                    std::cout<<"Receive the frame failed."<<std::endl;\
                    break;\
                }\
                {

#define EndDecode(cc,pkt,frm)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
        }

#define StartDecodeFlush(cc,frm)\
        {\
            avcodec_send_packet(cc,nullptr);\
            int ret_dec_flu=0;\
            while(ret_dec_flu>=0)\
            {\
                ret_dec_flu=avcodec_receive_frame(cc,frm);\
                if(ret_dec_flu==AVERROR(EAGAIN)||ret_dec_flu==AVERROR_EOF)\
                    break;\
                else if(ret_dec_flu<0)\
                {\
                    std::cout<<"Receive the frame failed."<<std::endl;\
                    break;\
                }\
                {

#define EndDecodeFlush(cc,frm)\
                }\
                av_packet_unref(((AVPacket*)(pkt)));\
            }\
        }

#endif //ANDROMEDA_MEDIA_CODEC
