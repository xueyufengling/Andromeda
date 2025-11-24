#ifndef ANDROMEDA_APP_APPLICATION
#define ANDROMEDA_APP_APPLICATION

#include <atomic>

#include "../thread/coroutine_lock.h"
#include "../thread/thread.h"
#include "frame_rate.h"
#include "../traits/Types.h"
#include "../util/log.h"

// @formatter:off
/**
 * 抽象的应用类，只包含更新逻辑，无图形渲染、音频等，可用于编写服务端
 */

#ifndef HAS_FUNC_INITIALIZE
#define HAS_FUNC_INITIALIZE
def_cls_has_func(initialize)
#endif//#HAS_FUNC_INITIALIZE#ifndef HAS_FUNC_PREINITIALIZE
#define HAS_FUNC_PREINITIALIZE
def_cls_has_func(preinitialize)
#endif//#HAS_FUNC_PREINITIALIZE#ifndef HAS_FUNC_UPDATE#define HAS_FUNC_UPDATEdef_cls_has_func(update)#endif//HAS_FUNC_UPDATE
#ifndef HAS_FUNC_RENDER_UPDATE
#define HAS_FUNC_RENDER_UPDATE
def_cls_has_func(render_update)
#endif//#HAS_FUNC_RENDER_UPDATE#ifndef HAS_FUNC_TERMINATE#define HAS_FUNC_TERMINATEdef_cls_has_func(terminate)#endif//HAS_FUNC_TERMINATE

//所有andromeda::app::Application（包括其本身）必须添加的friend class
#define DefineApplication(Derived) \
	friend class has_func(initialize)<void>;\
	friend class has_func(preinitialize)<void>;\
	friend class has_func(terminate)<void>;\
	friend class has_func(update)<void,float>;\
	friend class has_func(render_update)<void,float>;\
	friend class andromeda::app::main_loop_thread<Derived>;\
	friend class andromeda::app::application<Derived>;

//设置andromeda::app::Application为友元并导入其函数，所有直接继承子类都需要有
#define ImportApplicationAPI(Derived) \
	using andromeda::app::application<Derived>::launch;\
	using andromeda::app::application<Derived>::exit;


namespace andromeda
{
extern bool use_opengl;
extern bool use_portaudio;

namespace app
{

template<typename Derived>
class main_loop_thread;
//主线程也是Coroutine，在客户端中负责渲染，可以与主循环更新线程轮流执行
template<typename Derived>
class application : public andromeda::thread::coroutine_lock
{
	 DefineApplication(application)

protected:
	class main_loop_thread : public andromeda::thread::thread<void(), main_loop_thread>, public andromeda::thread::coroutine_lock
	{
	DefineThread

	friend class andromeda::thread::thread<void(), main_loop_thread>;
	friend class andromeda::app::application<Derived>;

	using andromeda::thread::coroutine_lock::turn;

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
				turn(app);
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

	bool is_running = false;
	std::atomic<bool> synchronize_fps; //主线程(如果是窗口程序则是渲染线程)是否与该线程同步
	main_loop_thread* app_main_loop_thread = nullptr;

	using andromeda::thread::coroutine_lock::turn;

	//主线程函数，负责事件处理和更新
	__attribute__((always_inline)) inline void _preinitialize() //在初始化Application时调用（各系统初始化之前）
	{
		if(andromeda::traits::is_class<Derived>::result && has_func(preinitialize)<void>::check<Derived>::result)
			((Derived*)this)->preinitialize();
	}

	__attribute__((always_inline)) inline void _initialize() //在各系统初始化完成后调用
	{
		is_running = true;
		if(andromeda::traits::is_class<Derived>::result && has_func(initialize)<void>::check<Derived>::result)
			((Derived*)this)->initialize();
	}

	__attribute__((always_inline)) inline void _terminate()
	{
		if(andromeda::traits::is_class<Derived>::result && has_func(terminate)<void>::check<Derived>::result)
			((Derived*)this)->terminate();
		is_running=false;
	}

	__attribute__((always_inline)) inline void _update(float tpf)
	{
		if(andromeda::traits::is_class<Derived>::result && has_func(update)<void,float>::check<Derived>::result)
			((Derived*)this)->update(tpf);
	}

	__attribute__((always_inline)) inline void _render_update(float render_tpf)
	{
		if(andromeda::traits::is_class<Derived>::result && has_func(render_update)<void,float>::check<Derived>::result)
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
			turn(app_main_loop_thread);
		}
	}

	void launch()
	{
		_initialize();
		app_main_loop_thread->start();
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
