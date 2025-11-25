#ifndef ANDROMEDA_AUDIO_ALBASIC
#define ANDROMEDA_AUDIO_ALBASIC

extern "C"
{
#include <alc.h>
#include <al.h>
}

#include "../common/bindable_object.h"
#include "../common/log.h"

/**
 * @brief 调用OpenAL函数并返回操作的成功或错误码
 */
template<typename Ret, typename Callable, typename ...Args>
__attribute__((always_inline)) inline ALenum alCall(Callable callable, Ret* ret, Args ... args)
{
	if(ret)
		*ret = callable(args...);
	else
		callable(args...);
	return alGetError();
}

template<typename Callable, typename ...Args>
__attribute__((always_inline)) inline ALenum alCallVoid(Callable callable, Args ... args)
{
	callable(args...);
	return alGetError();
}

/**
 * @brief 调用OpenAL函数并返回操作的结果
 */
template<typename Callable, typename ...Args>
__attribute__((always_inline)) inline auto alCallRet(Callable callable, Args ... args) -> decltype(callable(args...))
{
	ALenum err_code = AL_NO_ERROR;
	decltype(callable(args...)) ret = callable(args...);
	if((err_code = alGetError()) != AL_NO_ERROR)
	{
		LogError("alCallRet failed. error code: ", err_code);
	}
	return ret;
}

inline ALboolean al_IsExtensionPresent(const char* ext_name)
{
	return alCallRet(&alIsExtensionPresent, ext_name);
}

inline ALenum al_GenBuffers(ALsizei n, ALuint* buffers)
{
	return alCallVoid(&alGenBuffers, n, buffers);
}

inline ALenum al_DeleteBuffers(ALsizei n, const ALuint* buffers)
{
	return alCallVoid(&alDeleteBuffers, n, buffers);
}

inline ALenum al_BufferData(ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq)
{
	return alCallVoid(&alBufferData, bid, format, data, size, freq);
}

inline ALenum al_GenSources(ALsizei n, ALuint* sources)
{
	return alCallVoid(&alGenSources, n, sources);
}

inline ALenum al_DeleteSources(ALsizei n, const ALuint* sources)
{
	return alCallVoid(&alDeleteSources, n, sources);
}

inline ALenum al_Sourcei(ALuint sid, ALenum param, ALint value)
{
	return alCallVoid(&alSourcei, sid, param, value);
}

inline ALenum al_Sourcef(ALuint sid, ALenum param, ALfloat value)
{
	return alCallVoid(&alSourcef, sid, param, value);
}

inline ALenum al_Source3f(ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
	return alCallVoid(&alSource3f, sid, param, value1, value2, value3);
}

inline ALenum al_SourcePlay(ALuint sid)
{
	return alCallVoid(&alSourcePlay, sid);
}

inline ALenum al_GetSourcei(ALuint sid, ALenum param, ALint* value)
{
	return alCallVoid(&alGetSourcei, sid, param, value);
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
	return alCallVoid(&alcDestroyContext, context);
}

inline ALenum alc_MakeContextCurrent(ALCcontext* context)
{
	return alCallVoid(&alcMakeContextCurrent, context);
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

template<typename Derived>
class al_object: public andromeda::common::bindable_object<ALuint, Derived>
{
public:
	al_object() = default;

	al_object(ALuint existed_id) :
			andromeda::common::bindable_object<ALuint, Derived>(existed_id)
	{

	}
};

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
