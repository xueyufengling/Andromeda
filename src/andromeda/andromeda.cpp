#include <openal/alc.h>
#include <openal/al.h>
#include <andromeda/util/log.h>

namespace andromeda
{
bool use_opengl = true;
bool use_openal = true;

namespace audio
{
ALCdevice* al_device = nullptr;
ALCcontext* al_context = nullptr;
ALboolean al_support_extension;
}

static void _load_openal()
{
	audio::al_device = alcOpenDevice(NULL);
	if(audio::al_device)
	{
		audio::al_context = alcCreateContext(audio::al_device, NULL);
		alcMakeContextCurrent(audio::al_context);
	}
	audio::al_support_extension = alIsExtensionPresent("EAX2.0");
	alGetError(); //清除错误缓冲区
}

static void _term_openal()
{
	audio::al_context = alcGetCurrentContext();
	audio::al_device = alcGetContextsDevice(audio::al_context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(audio::al_context);
	alcCloseDevice(audio::al_device);
}
}

using namespace andromeda;

static __attribute__((constructor)) void _process_entry()
{
	if(use_openal)
		_load_openal();
}

static __attribute__((destructor)) void _process_exit()
{
	if(use_openal)
		_term_openal();
}

