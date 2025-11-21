#ifndef ANDROMEDA_MEDIA_AUDIOCODEC
#define ANDROMEDA_MEDIA_AUDIOCODEC

#include <fstream>
#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>
#include "pcm_audio.h"

namespace andromeda
{
namespace media
{
inline int sizeof_sample(AVSampleFormat fmt)
{
	switch(fmt)
	{
	case AV_SAMPLE_FMT_S16:
		case AV_SAMPLE_FMT_S16P:
		return 16;
	case AV_SAMPLE_FMT_S32:
		case AV_SAMPLE_FMT_S32P:
		return 32;
	case AV_SAMPLE_FMT_S64:
		case AV_SAMPLE_FMT_S64P:
		return 64;
	case AV_SAMPLE_FMT_U8:
		case AV_SAMPLE_FMT_U8P:
		return 8;
	default:
		return -1;
	}
}

typedef struct audio_properties
{
	int bit_rate;
	int sample_rate;
	int sample_bit;
} audio_properties;

typedef struct audio_frame_info
{
	int bit_rate;
} audio_frame_info;

class audio_decoder
{
public:

protected:
	audio_properties* audio_properties;
};

}
}

#endif // ANDROMEDA_MEDIA_AUDIOCODEC
