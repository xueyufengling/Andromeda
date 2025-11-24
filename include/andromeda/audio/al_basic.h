#ifndef ANDROMEDA_AUDIO_ALBASIC
#define ANDROMEDA_AUDIO_ALBASIC

extern "C"
{
#include <alc.h>
#include <al.h>
}

namespace andromeda
{
namespace audio
{
/**
 * @brief 一个音频输出设备的上下文
 */
class audio_device_context
{
protected:
	ALCdevice* al_device = nullptr;
	ALCcontext* al_context = nullptr;
	ALboolean al_support_extension = true;

public:
	inline operator ALCcontext*()
	{
		return al_context;
	}

	inline operator ALCdevice*()
	{
		return al_device;
	}

	inline bool is_support_extension()
	{
		return al_support_extension;
	}

	/**
	 * @brief 将本对象的AL上下文绑定为当前操作上下文
	 */
	inline static void bind(ALCcontext* al_context)
	{
		alcMakeContextCurrent(al_context);
	}

	/**
	 * @brief 当前绑定的AL上下文
	 */
	inline static ALCcontext* current_bind()
	{
		return alcGetCurrentContext();
	}

	/**
	 * @brief 获取指定AL上下文对应的设备
	 */
	inline static ALCdevice* context_device(ALCcontext* context)
	{
		return alcGetContextsDevice(context);
	}

	bool open_device(const char* dev_name = nullptr);

	void close_device();
};

}
}

#endif //ANDROMEDA_AUDIO_ALBASIC
