#ifndef _CXXACTX_AUDIOBUFFER
#define _CXXACTX_AUDIOBUFFER

#include <cxxactx/al_basic.h>

namespace andromeda
{
namespace audio
{
/**
 * @brief 音频PCM数据，可用于audio_source中播放
 */
class audio_buffer: public al_object<audio_buffer>
{
public:
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
};

}
}

#endif //_CXXACTX_AUDIOBUFFER
