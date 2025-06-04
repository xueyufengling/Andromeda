#ifndef ANDROMEDA_APP_FRAMERATE
#define ANDROMEDA_APP_FRAMERATE

#include <chrono>
#include "../util/timer.h"

namespace andromeda {
	namespace app {
		class FrameRate
		{
		private:
			andromeda::util::high_resolution_time_point now_time,past_time;
			double delta_t=0,tpf=0,tpf_limit=-1;
			bool limit_fps=false;
			int fps=0,fps_count=0,fps_limit=0,tpf_limit_nano_sec=-1; //fps_limit为最大帧率限制（<=0表示不限制），在调用calc()时如果单帧的时间不足1/fps_limit秒则会sleep该线程直到该帧的秒数占用达到1/fps_limit秒
		public:

			void init(); //初始化调用，只调用一次。再次调用则相当于重置时钟回归成第一次调用init()后的状态
			void calc(); //在主循环里执行更新操作完毕后调用，计算距离上次调用该函数（或init()调用后）经过的时间tpf，每调用一次fps+1。delta_t累计一秒后fps置零，可选在限制FPS时是否阻塞该线程，以达到限制FPS的功能

			__attribute__((always_inline)) inline double get_tpf()
			{
				return tpf;
			}

			__attribute__((always_inline)) inline int get_fps()
			{
				return fps;
			}

			__attribute__((always_inline)) inline int get_fps_count()//获取当前所在帧，如果fps_count为0则为第一帧，故加1
			{
				return fps_count+1;
			}

			void set_fps_limit(int _fps_limit);

			__attribute__((always_inline)) inline int get_fps_limit()
			{
				return fps_limit;
			}
		};
	}
}

#endif//ANDROMEDA_APP_FRAMERATE
