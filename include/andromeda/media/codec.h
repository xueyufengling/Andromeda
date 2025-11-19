#ifndef ANDROMEDA_MEDIA_CODEC
#define ANDROMEDA_MEDIA_CODEC

#include <functional>
#include <andromeda/util/log.h>
#include <andromeda/util/linked_buffer.h>

extern "C"
{
#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>
}

namespace andromeda
{
namespace media
{

typedef std::function<void(AVCodecContext*, AVFormatContext*, AVPacket*)> packet_proc;
typedef std::function<void(AVCodecContext*, AVFormatContext*, AVFrame*)> frame_proc;

void encode(AVCodecContext* codec_context, AVFormatContext* fmt_context, AVFrame* avframe, AVPacket*);

class codec
{
protected:
	AVCodec* avcodec = nullptr;
	AVCodecContext* codec_context = nullptr;
	AVFormatContext* fmt_context = nullptr;
	AVFrame* frame = nullptr;
	AVPacket* packet = nullptr;
	std::ofstream* output_file = nullptr;

	bool is_buf_alloc;
	long int encoded_frame_count, packet_count;

public:
	typedef std::function<void(AVCodecContext*, AVFormatContext*, AVFrame*, AVPacket*)> codec_proc;

	/**
	 * @brief 初始化编解码器
	 */
	bool initialize(AVCodecID id);

	void terminate();

	template<typename T>
	T& decode(const char* file, codec_proc proc)
	{
		int ret = 0;
		if((ret = avformat_open_input(&fmt_context, file, nullptr, nullptr)) < 0)
		{
			LogError(ret, "codec open input file failed.");
			return nullptr;
		}
		if((ret = avformat_find_stream_info(fmt_context, nullptr)) < 0)
		{
			LogError(ret, "codec find stream info failed.");
			return nullptr;
		}
		andromeda::util::linked_buffer<T> results(fmt_context->nb_streams);
		frame = av_frame_alloc();
		if(!frame)
		{
			LogError(ret, "codec allocate frame failed.");
			return nullptr;
		}
		packet = av_packet_alloc();
		if(!packet)
		{
			LogError(ret, "codec allocate packet failed.");
			return nullptr;
		}
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
