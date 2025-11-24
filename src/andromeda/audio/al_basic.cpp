#include <andromeda/audio/al_basic.h>

#include <andromeda/util/log.h>

using namespace andromeda::audio;

bool audio_device_context::open_device(const char* dev_name)
{
	if(!(al_device = alcOpenDevice(dev_name)))
	{
		LogError("open audio device ", dev_name, " failed");
		goto FAILED;
	}
	if(!(al_context = alcCreateContext(al_device, nullptr)))
	{
		LogError("create audio context for ", dev_name, " failed");
		goto FAILED;
	}
	al_support_extension = alIsExtensionPresent("EAX2.0");
	alGetError(); //清除错误缓冲区
	return true;
	FAILED:
	close_device();
	return false;
}

void audio_device_context::close_device()
{
	alcDestroyContext(al_context);
	al_context = nullptr;
	alcCloseDevice(al_device);
	al_device = nullptr;
}
