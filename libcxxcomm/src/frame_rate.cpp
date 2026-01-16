#include <cxxcomm/frame_rate.h>
#include <cxxcomm/log.h>

#include <thread>

using namespace cxxcomm;

#define DEBUG false

void frame_rate::init()
{
	delta_t = 0;
	tpf = 0;
	fps = 0;
	fps_count = 0;
	previous_time = HRC_TIME_NOW;
}

void frame_rate::set_fps_limit(int _fps_limit)
{
	fps_limit = _fps_limit;
	if(_fps_limit <= UNLIMITED)
		limit_fps = false;
	else
	{
		limit_fps = true;
		tpf_max_limit = 1.0 / fps_limit; //只要fps_limit>0，则计算每帧时间
		tpf_max_limit_nano = (int)(tpf_max_limit * 1E9);
	}
}

void frame_rate::calc()
{
	++fps_count;
	current_time = HRC_TIME_NOW;
	int tpf_nano_sec = HRC_TIME_DURATION_TO_NANOSEC(current_time - previous_time);
	tpf = tpf_nano_sec / 1E9;
	if(limit_fps)
	{
		delta_t += tpf_max_limit;
		previous_time += std::chrono::nanoseconds(tpf_max_limit_nano);
		std::chrono::nanoseconds sleep_time = std::chrono::nanoseconds(tpf_max_limit_nano - tpf_nano_sec); //sleep()以后浪费的额外时间算入下一帧花费的时间，保证每帧起始时间点都是tpf_limit的整数倍
#if DEBUG
		LogDebugInfo("frame_rate limit: ", fps_limit, ", sleep for ", (sleep_time.count()/1E6), "ms");
#endif
		if((sleep_time.count() > 0))
		{
			std::this_thread::sleep_for(sleep_time);
		}
	}
	else
	{
		delta_t += tpf;
		previous_time = current_time;
	}
	if(delta_t >= 1)
	{
		--delta_t;
		fps = fps_count;
		fps_count = 0;
	}
}
