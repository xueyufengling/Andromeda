#ifndef ANDROMEDA_MEDIA_VIDEOFACTORY
#define ANDROMEDA_MEDIA_VIDEOFACTORY

#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>
#include <iostream>

namespace andromeda {
	namespace media {
		enum output_mode
		{
			OUTPUT_NONE=-1, OUTPUT_VIDEO_BUFFER, OUTPUT_VIDEO_FILE, OUTPUT_BITSTREAM_BUFFER, OUTPUT_BITSTREAM_FILE
		};

		typedef void (*packet_proc)(AVCodecContext*,AVFormatContext*,AVPacket*);
		typedef void (*frame_proc)(AVCodecContext*,AVFormatContext*,AVFrame*);

		void encode(AVCodecContext *cc,AVFrame *frm,AVFormatContext* fc,AVStream *st,AVPacket *pkt,long int* count,packet_proc p);
		void encode_flush(AVCodecContext *cc,AVFormatContext* fc,AVStream *st,AVPacket *pkt,packet_proc p);
		void decode(AVCodecContext *cc,AVPacket* pkt,AVFormatContext* fc,AVFrame *frm,frame_proc p);
		void decode_flush(AVCodecContext *cc,AVFormatContext* fc,AVFrame *frm,frame_proc p);

		inline char* getErrMsg(int err_code,char* msg_buf=new char[AV_ERROR_MAX_STRING_SIZE])
		{
			av_make_error_string(msg_buf,AV_ERROR_MAX_STRING_SIZE,err_code);
			return msg_buf;
		}

		inline void logError(int err_code,const char* description=nullptr)
		{
			static char* msg_buf=new char[AV_ERROR_MAX_STRING_SIZE];
			std::cerr<<(description==nullptr?"Error: ":description)<<getErrMsg(err_code,msg_buf)<<std::endl;
		}

		inline void logEncodingStatus(AVFrame* frm,AVPacket* pkt)
		{
			std::cout<<"Writing Frame "<<frm->pts<<" : pts = "<<(pkt->pts)<<" , dts = "<<(pkt->dts)<<" ."<<std::endl;
		}

		void addStream(AVFormatContext *fc,AVCodec* c,int num);
		int addStreamUpTo(AVFormatContext *fc,AVCodec* c,int lim);
		void initVideoStream(AVFormatContext *fc,AVCodecContext *cc,AVCodec *c,int pos);
	}
}
#endif // ANDROMEDA_MEDIA_VIDEOFACTORY
