#ifndef _CXXMMPROC_CODEC
#define _CXXMMPROC_CODEC

/**
 * 此头文件依赖于ffmpeg库，在msys2中使用以下指令安装
 * pacman -S mingw-w64-ucrt-x86_64-ffmpeg
 */

#include <cxxcomm/array.h>
#include <cxxcomm/log.h>
#include <functional>
#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace cxxmmproc
{
typedef std::function<void(void*, AVCodecContext*, AVPacket*)> packet_proc;
typedef std::function<bool(AVCodecContext*)> codec_init;

extern codec_init no_extra_codec_init;

/**
 * @brief 打开一个文件输入上下文
 */
__attribute__((warn_unused_result)) AVFormatContext* open_input_file_context(const char* file);

void init_fmt_context_from_codec_context(AVFormatContext* fmt_context, AVCodecContext* codec_context, size_t stream_idx);

template<int StreamNum>
inline void _alloc_output_file_context(AVFormatContext* fmt_context)
{
}

template<int StreamNum, typename C, typename ...Cs>
inline void _alloc_output_file_context(AVFormatContext* fmt_context, C codec_context, Cs* ... codec_contexts)
{
	init_fmt_context_from_codec_context(fmt_context, codec_context, (StreamNum - sizeof...(Cs) - 1));
	_alloc_output_file_context<StreamNum>(codec_contexts...);
}

/**
 * @brief 打开一个文件输出上下文
 */
template<typename ...Cs>
__attribute__((warn_unused_result)) AVFormatContext* alloc_output_file_context(const char* file, Cs* ... codec_contexts)
{
	AVFormatContext* fmt_context = avformat_alloc_context();
	if(!fmt_context)
	{
		LogError("codec allocate output format context for file ", file, " failed");
		return nullptr;
	}
	_alloc_output_file_context<sizeof...(Cs)>(fmt_context, codec_contexts...); //将codec的参数按顺序拷贝进AVFormatContext
	strcpy(fmt_context->url, file);
	fmt_context->oformat = av_guess_format(nullptr, file, nullptr);
	return fmt_context;
}

__attribute__((warn_unused_result)) AVFormatContext* alloc_output_file_context(const char* file, AVCodecContext** codec_contexts, size_t context_num);

/**
 * @brief 关闭一个文件上下文
 */
void close_file_context(AVFormatContext* fmt_context);

/**
 * 为指定缓冲变量赋值并初始化
 */
bool alloc_send_recv_buffer(AVPacket** packet, AVFrame** frame);

/**
 * @brief 从AVFormatContext中读取下一个AVPacket数据，带有位置记录
 */
AVPacket* read_packet(AVFormatContext* fmt_context);

template<typename ...Args>
__attribute__((warn_unused_result)) int decode_packet(AVCodecContext* decoder_context, AVPacket* raw_packet, AVFrame* decoded_frame, bool unref_packet, std::function<void(AVCodecContext*, AVFrame*, Args...)> proc, Args ...args)
{
	int ret = 0;
	if((ret = avcodec_send_packet(decoder_context, raw_packet)) < 0)
	{
		if(raw_packet)
		{
			LogError("codec send the packet failed. error code: ", ret);
		}
		else
		{
			LogError("codec flush the packet failed. error code: ", ret);
		}
		goto FAILED;
	}
	while(ret >= 0)
	{
		ret = avcodec_receive_frame(decoder_context, decoded_frame);
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) //全部帧读取完毕
			break;
		else if(ret < 0)
		{
			LogError("codec receive the frame failed");
			goto FAILED;
		}
		else if(proc)
			proc(decoder_context, decoded_frame, args...);
	}
	if(unref_packet)
		av_packet_unref(raw_packet);
	return ret;
	FAILED:
	avcodec_send_packet(decoder_context, nullptr); //遇到错误则清空缓冲区
	return ret;
}

template<typename ...Args>
inline int decode_packet_flush(AVCodecContext* decoder_context, AVFrame* decoded_frame, std::function<void(AVCodecContext*, AVFrame*, Args...)> proc, Args ...args)
{
	return decode_packet(decoder_context, nullptr, decoded_frame, false, proc, args...); //如果AVPacket*是nullptr则对解码缓冲区剩余内容解码，并清空缓冲区
}

//编码
class encoder
{
protected:
	AVCodecID encoder_id = AVCodecID::AV_CODEC_ID_NONE;
	AVCodec* encoder_codec = nullptr;
	AVCodecContext* encoder_context = nullptr;
	AVFrame* raw_frame = nullptr;
	AVPacket* encoded_packet = nullptr;

	int64_t encoded_frame_count, //已编码的帧的数量，用作时间戳
			packet_count;

	bool allocate_encode_buffer();

public:

	inline AVCodecID get_encoder_id()
	{
		return encoder_id;
	}

	/**
	 * @brief 初始化编码器
	 */
	bool initialize_encoder(AVCodecID codec_id, codec_init init_proc = codec_init());

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
	AVCodecParserContext* decoder_parser = nullptr; //将内存数据转为AVPacket供解码
	AVFrame* decoded_frame = nullptr;
	AVPacket* raw_packet = nullptr;

	int64_t encoded_frame_count, //已解码的帧的数量
			packet_count;

	bool allocate_decode_buffer();

public:
	inline AVCodecID get_decoder_id()
	{
		return decoder_id;
	}

	/**
	 * @brief 初始化解码器，包括查找AVCodec、给其分配AVCodecContext、AVFrame缓冲
	 */
	bool initialize_decoder(AVCodecID codec_id, codec_init init_proc = codec_init());

	/**
	 * @brief 初始化解码器，包括查找AVCodec、给其分配AVCodecContext、AVFrame缓冲，并将codecpar拷贝给AVCodecContext上下文
	 */
	bool initialize_decoder(AVCodecParameters* codecpar, codec_init init_proc = codec_init());

	inline bool initialize_decoder(AVStream* stream, codec_init init_proc = codec_init())
	{
		return initialize_decoder(stream->codecpar, init_proc);
	}

	/**
	 * @brief 打开解码器
	 */
	bool open_decoder();

	void terminate_decoder();

	/**
	 * @brief 以指定的内存数据填充AVPacket，不带位置记录，每次需要手动计算新的data和size
	 * @param packet 接收数据的AVPacket
	 * @param data 待转换的内存数据
	 * @param size 待转换的内存数据总长度
	 * @return 该packet读取转换的字节数
	 */
	int read_packet(AVPacket* packet, void* data, size_t size);

	template<typename ...Args>
	inline int decode(AVPacket* packet, bool unref_packet, std::function<void(AVCodecContext*, AVFrame*, Args...)> proc, Args ...args)
	{
		return decode_packet(decoder_context, packet, decoded_frame, unref_packet, proc, args...);
	}

	/**
	 * @brief 对本解码器的缓冲区剩余数据进行解码
	 */
	template<typename ...Args>
	inline int decode_flush(std::function<void(AVCodecContext*, AVFrame*, Args...)> proc, Args ...args)
	{
		return decode_packet_flush(decoder_context, decoded_frame, proc, args...); //如果AVPacket*是nullptr则对解码缓冲区剩余内容解码，并清空缓冲区
	}

	/**
	 * @brief 解码全部内存数据
	 */
	template<typename ...Args>
	int decode(void* data, size_t size, std::function<void(AVCodecContext*, AVFrame*, Args...)> proc, Args ...args)
	{
		int ret = 0;
		unsigned char* mem_data = (unsigned char*)data;
		while(ret >= 0)
		{
			if((ret = read_packet(raw_packet, mem_data, size)) < 0)
			{
				return ret;
			}
			if((ret = decode(raw_packet, true, proc, args...)) < 0)
			{
				return ret;
			}
			mem_data += ret;
			size -= ret;
		}
		ret = decode_flush(proc, args...);
		return ret;
	}

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
		return stream(idx)->codecpar->codec_type;
	}
};

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

#endif //_CXXMMPROC_CODEC
