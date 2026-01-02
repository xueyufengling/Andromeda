#ifndef ANDROMEDA_AUDIO_ALBASIC
#define ANDROMEDA_AUDIO_ALBASIC

extern "C"
{
#include <alc.h>
#include <al.h>
}

#include "../common/bindable_object.h"
#include "../common/lib_call.h"

decl_libcall(al, ALenum, alGetError, AL_NO_ERROR)

#define al_call(callable, ...) libcall(al, __log_source__, callable, ##__VA_ARGS__)

#define al_IsExtensionPresent(ext_name) al_call(alIsExtensionPresent, ext_name)

#define al_GenBuffers(n, buffers) al_call(alGenBuffers, n, buffers)

#define al_DeleteBuffers(n, buffers) al_call(alDeleteBuffers, n, buffers)

#define al_BufferData(bid, format, data, size, freq) al_call(alBufferData, bid, format, data, size, freq)

#define al_GenSources(n, sources) al_call(alGenSources, n, sources)

#define al_DeleteSources(n, sources) al_call(alDeleteSources, n, sources)

#define al_Sourcei(sid, param, value) al_call(alSourcei, sid, param, value)

#define al_Sourcef(sid, param, value) al_call(alSourcef, sid, param, value)

#define al_Source3f(sid, param, value1, value2, value3) al_call(alSource3f, sid, param, value1, value2, value3)

#define al_SourcePlay(sid) al_call(alSourcePlay, sid)

#define al_GetSourcei(sid, param, value) al_call(alGetSourcei, sid, param, value)

#define alc_OpenDevice(devicename) al_call(alcOpenDevice, devicename)

#define alc_CloseDevice(device) al_call(alcCloseDevice, device)

#define alc_CreateContext(device, attrlist) al_call(alcCreateContext, device, attrlist)

#define alc_DestroyContext(context) al_call(alcDestroyContext, context)

#define alc_MakeContextCurrent(context) al_call(alcMakeContextCurrent, context)

#define alc_GetCurrentContext() al_call(alcGetCurrentContext)

#define alc_GetContextsDevice(context) al_call(alcGetContextsDevice, context)

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
