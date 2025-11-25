#include <andromeda/common/timer.h>

using namespace andromeda::common;

void timer::init()
{
	delta_t=0;
	past_time=HRC_TIME_NOW;
}

void timer::calc()
{
	now_time=HRC_TIME_NOW;
	delta_t+=HRC_TIME_DURATION_TO_NANOSEC(now_time-past_time);
	past_time=now_time;
	long long int interval=intervals[interval_idx_counter];
	if(delta_t>=interval)
	{
		delta_t-=interval;
		++interval_idx_counter;
		++counter;
		if(callback)
			callback(interval,counter,interval_idx_counter);
	}
}
