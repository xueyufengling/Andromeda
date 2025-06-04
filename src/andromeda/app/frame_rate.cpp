#include <andromeda/app/frame_rate.h>

#include <thread>

using namespace andromeda::app;
using namespace andromeda::util;
using namespace std::chrono;

void FrameRate::init()
{
	delta_t=0;
	tpf=0;
	fps=0;
	fps_count=0;
	past_time=HRC_TIME_NOW;
}

void FrameRate::set_fps_limit(int _fps_limit)
{
	fps_limit=_fps_limit;
	if(_fps_limit<=0)
		limit_fps=false;
	else
	{
		limit_fps=true;
		tpf_limit=1.0/fps_limit; //只要fps_limit>0，则计算每帧时间
		tpf_limit_nano_sec=(int)(tpf_limit*1E9);
	}
}

void FrameRate::calc()
{
	++fps_count;
	now_time=HRC_TIME_NOW;
	int tpf_nano_sec=HRC_TIME_DURATION_TO_NANOSEC(now_time-past_time);
	tpf=tpf_nano_sec/1E9;
	if(limit_fps)
	{
		delta_t+=tpf_limit;
		past_time+=nanoseconds(tpf_limit_nano_sec);
		nanoseconds sleep_time=nanoseconds(tpf_limit_nano_sec-tpf_nano_sec); //sleep()以后浪费的额外时间算入下一帧花费的时间，保证每帧起始时间点都是tpf_limit的整数倍
#ifdef DEBUG_FrameRate
		std::cout<<"FrameRate sleep for "<<(sleep_time.count()/1E6)<<"ms"<<std::endl;
#endif
		if((sleep_time.count()>0))
		{
			std::this_thread::sleep_for(sleep_time);
		}
	}
	else
	{
		delta_t+=tpf;
		past_time=now_time;
	}
	if(delta_t>=1)
	{
		--delta_t;
		fps=fps_count;
		fps_count=0;
	}
}
