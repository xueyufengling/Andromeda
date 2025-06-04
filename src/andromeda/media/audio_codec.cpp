#include <andromeda/media/audio_codec.h>

using namespace andromeda::media;

AudioCodec::AudioCodec(AVCodecID id)
{
	construct(id);
}

AudioCodec::~AudioCodec()
{
	deconstruct();
}

bool AudioCodec::construct(AVCodecID id)
{
	codec=avcodec_find_encoder(id);
	codec_context=avcodec_alloc_context3(codec);
}

void AudioCodec::deconstruct()
{

}

void openOutputAudioFile(const char* name)
{

}
void closeOutputAudioFile()
{

}
void encodePCMStreamToAudioFile(const char* name,PCMAudio pcm)
{

}
