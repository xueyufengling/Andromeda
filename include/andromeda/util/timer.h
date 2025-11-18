#ifndef ANDROMEDA_UTIL_TIMER
#define ANDROMEDA_UTIL_TIMER

#include <chrono>
#include <functional>

#include "array_list.h"
#include "limit_counter.h"

//高精度时钟的time_point转换成秒
#define HRC_TIME_DURATION_TO_SEC(delta_time) ((double)std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count()/1E9)
#define HRC_TIME_DURATION_TO_MILSEC(delta_time) ((double)std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count()/1E6)
#define HRC_TIME_DURATION_TO_NANOSEC(delta_time) ((long long int)std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count())
#define HRC_TIME_NOW (std::chrono::high_resolution_clock::now())

namespace andromeda
{
namespace util
{
typedef std::chrono::time_point<std::chrono::high_resolution_clock> high_resolution_time_point; //采用的高精度时间点类型

//需要手动更新计算时间间隔的定时器，每次工作循环只更新一次，使用系统高精度时钟（纳秒）计时
class timer
{
private:
	high_resolution_time_point now_time, past_time;
	long long int delta_t = 0;
	size_t counter = 0; //每达到一次interval就计数一次
	limit_counter<size_t> interval_idx_counter;
	array_list<long long int> intervals;

public:
	std::function<void(float, size_t&, size_t&)> callback;

	void init(); //初始化调用，只调用一次。再次调用则相当于重置时钟回归成第一次调用init()后的状态
	void calc(); //在主循环里执行更新操作完毕后调用，计算距离上次调用该函数（或init()调用后）经过的时间delta_t，到达时间间隔后将回调

	template<typename ...Ts>
	__attribute__((always_inline)) inline void add_interval(Ts ... intervals_nanosec)
	{
		intervals.add(intervals_nanosec...);
		interval_idx_counter._end += sizeof...(Ts);
	}

	template<typename ...Ts>
	__attribute__((always_inline)) inline void set_interval(Ts ... intervals_nanosec)
	{
		remove_all_interval();
		add_interval(intervals_nanosec...);
	}

	__attribute__((always_inline)) inline void remove_interval(size_t rm_count)
	{
		intervals.remove(rm_count);
		interval_idx_counter._end -= rm_count;
	}

	__attribute__((always_inline)) inline void remove_all_interval()
	{
		intervals.clear();
		interval_idx_counter._end = -1;
	}
};
}
}

#endif//ANDROMEDA_UTIL_TIMER
