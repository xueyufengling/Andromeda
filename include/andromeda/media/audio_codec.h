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
class audio_codec
{
	friend class muxer;

public:
	typedef struct properties
	{
		int bit_rate;
		int sample_rate;
		int sample_bit;
	} properties;

	typedef struct frame_info
	{
		int bit_rate;
	} frame_info;

	audio_codec(AVCodecID id);
	~audio_codec();

	bool construct(AVCodecID id);
	void deconstruct();

protected:
	AVCodec* codec;
	AVCodecContext* codec_context;
	AVFrame* frame;
	AVPacket* packet;
	std::ofstream* output_file;
	properties* audio_properties;
	bool is_buf_alloc;
	long int encoded_frame_count;
};
}
}

#endif // ANDROMEDA_MEDIA_AUDIOCODEC
