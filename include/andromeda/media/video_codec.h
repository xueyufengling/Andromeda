#ifndef ANDROMEDA_MEDIA_VIDEOCODEC
#define ANDROMEDA_MEDIA_VIDEOCODEC

#include <fstream>
#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>

#include "raster_image.h"

namespace andromeda {
	namespace media {
		class VideoProperties
		{
		public:
			int width,height;
			int frame_rate_num,frame_rate_den;
			int timebase_num=0,timebase_den=0;
			int bit_rate;
			int gop_size;
			int max_b_frames;
			int nb_video_stream,nb_audio_stream,nb_subtitle_stream;
		};

		class VideoFrameInfo
		{
		public:
			andromeda::media::RasterImage* img;
			int last_frame_count;

			VideoFrameInfo(andromeda::media::RasterImage& image);
			VideoFrameInfo(andromeda::media::RasterImage& image,int lfc);
		};

		class VideoCodec
		{
			friend class Muxer;

		protected:
			AVCodec* codec;
			AVCodecContext* codec_context;
			AVFrame* frame;
			AVPacket* packet;
			std::ofstream* output_file;
			VideoProperties* properties;
			bool is_buf_alloc;
			long int encoded_frame_count,packet_count;

			bool init_codec(AVCodecID id);
			bool construct(AVCodecID id);
			void deconstruct();
		public:
			VideoCodec(AVCodecID id);
			~VideoCodec();

			void inputFrame(andromeda::media::RasterImage& img,int last_frame_count);
			void inputFrames(VideoFrameInfo* infos,int length);

			void applySettings(void);

			void openOutputBitstreamFile(const char* file);
			void closeOutputBitstreamFile(void);

			inline void setFrameSize(int width,int height)
			{
				properties->width=width;
				properties->height=height;
			}

			inline void setFrameRate(int numerator,int denominator)
			{
				properties->frame_rate_num=numerator;
				properties->frame_rate_den=denominator;
			}

			inline void setTimeBase(int numerator,int denominator)
			{
				properties->timebase_num=numerator;
				properties->timebase_den=denominator;
			}

			inline void setBitRate(int rate)
			{
				properties->bit_rate=rate;
			}

			inline void setGopSize(int gopsize)
			{
				properties->gop_size=gopsize;
			}

			inline void setMaxBFrames(int max)
			{
				properties->max_b_frames=max;
			}

			inline void setProperties(VideoProperties* pro)
			{
				properties=pro;
			}

			static void loadYUV444PDataFromRasterImageToFrame(andromeda::media::RasterImage& img,AVFrame* frame_);
		};
	}
}

#endif // ANDROMEDA_MEDIA_VIDEOCODEC
