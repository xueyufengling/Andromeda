#ifndef ANDROMEDA_APP_APPLICATION
#define ANDROMEDA_APP_APPLICATION

#include <atomic>

#include "../common/log.h"
#include "../thread/thread.h"
#include "../thread/sequential_lock.h"
#include "frame_rate.h"

#include "../common/intl_memb_detect.h"

// @formatter:off

//所有andromeda::app::application必须添加的friend class
#define ConstructApplication(Derived)\
		enable_exist_memb_with_type(preinitialize, initialize, update, render_update, terminate)\
		friend class andromeda::app::application<Derived>;\
		friend class andromeda::app::application<Derived>::main_loop_thread;

namespace andromeda
{
namespace app
{
template<typename Derived>
class window_application;

/**
 * 抽象的应用类，只包含更新逻辑，无图形渲染、音频等，可用于编写服务端
 */
template<typename Derived>
class application : public andromeda::thread::sequential_lock
{
	enable_exist_memb_with_type(preinitialize, initialize, update, render_update, terminate)

protected:
	class main_loop_thread: public andromeda::thread::thread<void(), main_loop_thread>, public andromeda::thread::sequential_lock
	{
	friend class andromeda::thread::thread<void(), main_loop_thread>;
	friend class andromeda::app::application<Derived>;
	friend class andromeda::app::window_application<Derived>;

	using andromeda::thread::sequential_lock::swap;

	protected:
		Derived* app;
		frame_rate update_rate;

		void initialize()
		{
			update_rate.init();
		}

		void run() //多线程执行的更新逻辑函数
		{
			app->_update(update_rate.get_tpf());
			update_rate.calc();
			if(app->synchronize_fps)
				swap(app);
		}

		void terminate()
		{

		}

	public:
		main_loop_thread(Derived *derived_app) :
				andromeda::thread::thread<void(), main_loop_thread>(&(derived_app->is_running), andromeda::thread::work_mode::DETACH)
		{
			app = derived_app;
		}

		inline void set_update_rate_limit(int update_fps)
		{
			update_rate.set_fps_limit(update_fps);
		}

		inline int get_update_rate()
		{
			return update_rate.get_fps();
		}

		inline int get_update_rate_count()
		{
			return update_rate.get_fps_count();
		}

		using andromeda::thread::thread<void(),main_loop_thread>::start;
		using andromeda::thread::thread<void(),main_loop_thread>::stop;
	};

	volatile bool is_running = false;
	std::atomic<bool> synchronize_fps; //主线程(如果是窗口程序则是渲染线程)是否与该线程同步
	main_loop_thread* app_main_loop_thread = nullptr;

	using andromeda::thread::sequential_lock::swap;

	//主线程函数，负责事件处理和更新
	__attribute__((always_inline)) inline void _preinitialize() //在初始化Application时调用（各系统初始化之前）
	{
		if(is_class<Derived>::result && exist_memb_with_type(Derived, void(), preinitialize)::result)
			((Derived*)this)->preinitialize();
	}

	__attribute__((always_inline)) inline void _initialize() //在各系统初始化完成后调用
	{
		is_running = true;
		if(is_class<Derived>::result && exist_memb_with_type(Derived, void(), initialize)::result)
			((Derived*)this)->initialize();
	}

	__attribute__((always_inline)) inline void _terminate()
	{
		if(is_class<Derived>::result && exist_memb_with_type(Derived, void(), terminate)::result)
			((Derived*)this)->terminate();
		is_running=false;
	}

	__attribute__((always_inline)) inline void _update(float tpf)
	{
		if(is_class<Derived>::result && exist_memb_with_type(Derived, void(float), update)::result)
			((Derived*)this)->update(tpf);
	}

	__attribute__((always_inline)) inline void _render_update(float render_tpf)
	{
		if(is_class<Derived>::result && exist_memb_with_type(Derived, void(float), render_update)::result)
			((Derived*)this)->render_update(render_tpf);
	}

public:
	application()
	{
		app_main_loop_thread = new main_loop_thread((Derived*)this);
		set_synchronize_fps(false);
		_preinitialize(); //可以调用glfwWindowHint()，不可设置窗口参数、调用OpenGL函数，否则空指针异常
	}

	~application()
	{
		delete app_main_loop_thread;
	}

	__attribute__((always_inline)) inline void _launch_main_loop()
	{
		while(is_running)
		{
			//更新
			swap(app_main_loop_thread);
		}
	}

	void launch()
	{
		_initialize();
		app_main_loop_thread->start();
		if(is_class<Derived>::result && exist_memb_with_type(Derived, void(), _launch_main_loop)::result)
			((Derived*)this)->_launch_main_loop();
		_terminate();
	}

	void exit()
	{
		is_running = false;
	}

	__attribute__((always_inline)) inline void set_synchronize_fps(bool synchronize_fps)
	{
		this->synchronize_fps = synchronize_fps;
	}

	__attribute__((always_inline)) inline int get_update_rate()
	{
		return app_main_loop_thread->get_update_rate();
	}

	__attribute__((always_inline)) inline int get_update_rate_count() //获取当前所在帧
	{
		return app_main_loop_thread->get_update_rate_count();
	}

	__attribute__((always_inline)) inline void set_update_rate_limit(int ur_limit)
	{
		app_main_loop_thread->set_update_rate_limit(ur_limit);
	}
};

}
}
// @formatter:on

#endif//ANDROMEDA_APP_APPLICATION
