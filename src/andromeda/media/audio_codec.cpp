#include <andromeda/media/audio_codec.h>

using namespace andromeda::media;

audio_codec::audio_codec(AVCodecID id)
{
	construct(id);
}

audio_codec::~audio_codec()
{
	deconstruct();
}

bool audio_codec::construct(AVCodecID id)
{
	codec = avcodec_find_encoder(id);
	codec_context = avcodec_alloc_context3(codec);
}

void audio_codec::deconstruct()
{

}
