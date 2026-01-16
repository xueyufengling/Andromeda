#ifndef _CXXCOMM_FRAMERATE
#define _CXXCOMM_FRAMERATE

#include <cxxcomm/timer.h>
#include <chrono>

namespace cxxcomm
{
class frame_rate
{
private:
	cxxcomm::high_resolution_time_point current_time, previous_time;
	double delta_t = 0;
	double tpf = 0;
	double tpf_max_limit = 0; //开启帧率限制后每帧的最大时间，其值为1/fps_limit，在调用calc()时如果单帧的时间不足1/fps_limit秒则会sleep该线程直到该帧的秒数占用达到1/fps_limit秒
	int tpf_max_limit_nano = 0; //与tpf_max_limit值一致，但单位为ns，便于与high_resolution_time_point比较
	bool limit_fps = false;
	int fps = 0;
	int fps_count = 0;
	int fps_limit = 0; //fps_limit为最大帧率限制（<0表示不限制）

public:
	static constexpr int UNLIMITED = 0;

	frame_rate() = default;

	/**
	 * @brief 初始化调用，只调用一次。再次调用则相当于重置时钟回归成第一次调用init()后的状态
	 */
	void init();

	/**
	 * @brief 在主循环里执行更新操作完毕后调用，计算距离上次调用该函数（或init()调用后）经过的时间tpf，每调用一次fps+1。delta_t累计一秒后fps置零，可选在限制FPS时是否阻塞该线程，以达到限制FPS的功能
	 */
	void calc();

	__attribute__((always_inline)) inline double get_tpf()
	{
		return tpf;
	}

	__attribute__((always_inline)) inline int get_fps()
	{
		return fps;
	}

	/**
	 * @brief 获取当前所在帧，如果fps_count为0则为第一帧，故加1
	 */
	__attribute__((always_inline)) inline int get_fps_count()
	{
		return fps_count + 1;
	}

	void set_fps_limit(int _fps_limit);

	__attribute__((always_inline)) inline int get_fps_limit()
	{
		return fps_limit;
	}

	/**
	 * @brief 将此frame_rate的帧率限制同步给参数fr
	 */
	__attribute__((always_inline)) inline int synchronize_fps(frame_rate& fr)
	{
		fr.set_fps_limit(fps_limit);
		return fps_limit;
	}
};

}

#endif//_CXXCOMM_FRAMERATE
