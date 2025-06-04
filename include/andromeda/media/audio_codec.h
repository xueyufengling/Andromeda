#ifndef ANDROMEDA_MEDIA_AUDIOCODEC
#define ANDROMEDA_MEDIA_AUDIOCODEC

#include <fstream>
#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>
#include "pcm_audio.h"

namespace andromeda {
	namespace media {
		class AudioCodec
		{
			friend class Muxer;

		public:
			typedef struct AudioProperties
			{
				int bit_rate;
				int sample_rate;
				int sample_bit;
			} AudioProperties;

			typedef struct AudioFrameInfo
			{
				int bit_rate;
			} AudioFrameInfo;

			AudioCodec(AVCodecID id);
			~AudioCodec();

			bool construct(AVCodecID id);
			void deconstruct();

			void openOutputAudioFile(const char* name);
			void closeOutputAudioFile();
			void encodePCMStreamToAudioFile(const char* name,PCMAudio pcm);

		protected:
			AVCodec *codec;
			AVCodecContext *codec_context;
			AVFrame *frame;
			AVPacket *packet;
			std::ofstream* output_file;
			AudioProperties *properties;
			bool is_buf_alloc;
			long int encoded_frame_count;
		};
	}
}

#endif // ANDROMEDA_MEDIA_AUDIOCODEC
