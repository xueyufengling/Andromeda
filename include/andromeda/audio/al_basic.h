#ifndef ANDROMEDA_AUDIO_ALBASIC
#define ANDROMEDA_AUDIO_ALBASIC

extern "C"
{
#include <alc.h>
#include <al.h>
}

#include "../common/bindable_object.h"
#include <andromeda/common/lib_call.h>

LibCallAll(alCall, ALenum, alGetError, AL_NO_ERROR)

inline ALboolean al_IsExtensionPresent(const char* ext_name)
{
	return alCallRet(&alIsExtensionPresent, ext_name);
}

inline ALenum al_GenBuffers(ALsizei n, ALuint* buffers)
{
	return alCallRet(&alGenBuffers, n, buffers);
}

inline ALenum al_DeleteBuffers(ALsizei n, const ALuint* buffers)
{
	return alCallRet(&alDeleteBuffers, n, buffers);
}

inline ALenum al_BufferData(ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq)
{
	return alCallRet(&alBufferData, bid, format, data, size, freq);
}

inline ALenum al_GenSources(ALsizei n, ALuint* sources)
{
	return alCallRet(&alGenSources, n, sources);
}

inline ALenum al_DeleteSources(ALsizei n, const ALuint* sources)
{
	return alCallRet(&alDeleteSources, n, sources);
}

inline ALenum al_Sourcei(ALuint sid, ALenum param, ALint value)
{
	return alCallRet(&alSourcei, sid, param, value);
}

inline ALenum al_Sourcef(ALuint sid, ALenum param, ALfloat value)
{
	return alCallRet(&alSourcef, sid, param, value);
}

inline ALenum al_Source3f(ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
	return alCallRet(&alSource3f, sid, param, value1, value2, value3);
}

inline ALenum al_SourcePlay(ALuint sid)
{
	return alCallRet(&alSourcePlay, sid);
}

inline ALenum al_GetSourcei(ALuint sid, ALenum param, ALint* value)
{
	return alCallRet(&alGetSourcei, sid, param, value);
}

inline ALCdevice* alc_OpenDevice(const ALCchar* devicename)
{
	return alCallRet(&alcOpenDevice, devicename);
}

inline ALCboolean alc_CloseDevice(ALCdevice* device)
{
	return alCallRet(&alcCloseDevice, device);
}

inline ALCcontext* alc_CreateContext(ALCdevice* device, const ALCint* attrlist)
{
	return alCallRet(&alcCreateContext, device, attrlist);
}

inline ALenum alc_DestroyContext(ALCcontext* context)
{
	return alCallRet(&alcDestroyContext, context);
}

inline ALenum alc_MakeContextCurrent(ALCcontext* context)
{
	return alCallRet(&alcMakeContextCurrent, context);
}

inline ALCcontext* alc_GetCurrentContext(void)
{
	return alCallRet(&alcGetCurrentContext);
}

inline ALCdevice* alc_GetContextsDevice(ALCcontext* context)
{
	return alCallRet(&alcGetContextsDevice, context);
}

namespace andromeda
{
namespace audio
{

__attribute__((always_inline)) inline ALint al_get_integer(ALuint source, ALuint param)
{
	ALint result = 0;
	al_GetSourcei(source, param, &result);
	return result;
}

inline ALint al_get_source_state(ALuint source)
{
	return al_get_integer(source, AL_SOURCE_STATE);
}

inline ALenum al_set_source_buffer(ALuint source, ALuint buffer)
{
	return al_Sourcei(source, AL_BUFFER, buffer);
}

inline ALenum al_set_source_loop(ALuint source, bool loop)
{
	return al_Sourcei(source, AL_LOOPING, loop);
}

inline ALenum al_set_source_gain(ALuint source, float gain)
{
	return al_Sourcef(source, AL_GAIN, gain);
}

inline ALenum al_set_source_pos(ALuint source, float x, float y, float z)
{
	return al_Source3f(source, AL_POSITION, x, y, z);
}

/**
 * @brief 一个音频输出设备的结构体
 */
class al_context: public andromeda::common::bindable_object<ALCcontext*, al_context>
{
protected:
	ALCdevice* al_device = nullptr;

public:
	inline static bool support_extension(const char* ext_name = "EAX2.0")
	{
		return al_IsExtensionPresent(ext_name);
	}

	inline operator ALCdevice*()
	{
		return al_device;
	}

	/**
	 * @brief 目标AL上下文绑定为当前操作上下文
	 */
	inline static void bind(ALCcontext* al_context)
	{
		alc_MakeContextCurrent(al_context);
	}

	/**
	 * @brief 当前绑定的AL上下文
	 */
	inline static ALCcontext* current_bind()
	{
		return alc_GetCurrentContext();
	}

	bool open_device(const char* dev_name = nullptr);

	void close_device();
};

}
}

#endif //ANDROMEDA_AUDIO_ALBASIC
