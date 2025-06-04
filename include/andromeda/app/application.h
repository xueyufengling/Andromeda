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
	friend class andromeda::app::MainLoopThread<Derived>;\
	friend class andromeda::app::Application<Derived>;

//设置andromeda::app::Application为友元并导入其函数，所有直接继承子类都需要有
#define ImportApplicationAPI(Derived) \
	using andromeda::app::Application<Derived>::launch;\
	using andromeda::app::Application<Derived>::exit;

namespace andromeda {
	extern bool use_opengl;
	extern bool use_portaudio;

	namespace app {
		template<typename Derived>
		class MainLoopThread;

		//主线程也是Coroutine，在客户端中负责渲染，可以与主循环更新线程轮流执行
		template<typename Derived>
		class Application:public andromeda::thread::CoroutineLock
		{
			DefineApplication(Application)
		protected:
			bool is_running=false;
			std::atomic<bool> synchronize_fps; //主线程(如果是窗口程序则是渲染线程)是否与该线程同步
			MainLoopThread<Derived>* main_loop_thread=nullptr;

			using andromeda::thread::CoroutineLock::turn;

			//主线程函数，负责事件处理和更新
			__attribute__((always_inline)) inline void _preinitialize() //在初始化Application时调用（各系统初始化之前）
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(preinitialize)<void>::check<Derived>::result)
					((Derived*)this)->preinitialize();
			}

			__attribute__((always_inline)) inline void _initialize() //在各系统初始化完成后调用
			{
				is_running=true;
				if(andromeda::traits::is_class<Derived>::result&&has_func(initialize)<void>::check<Derived>::result)
					((Derived*)this)->initialize();
			}

			__attribute__((always_inline)) inline void _terminate()
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(terminate)<void>::check<Derived>::result)
					((Derived*)this)->terminate();
				is_running=false;
			}

			__attribute__((always_inline)) inline void _update(float tpf)
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(update)<void,float>::check<Derived>::result)
					((Derived*)this)->update(tpf);
			}

			__attribute__((always_inline)) inline void _render_update(float render_tpf)
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(render_update)<void,float>::check<Derived>::result)
					((Derived*)this)->render_update(render_tpf);
			}

		public:
			Application()
			{
				main_loop_thread=new MainLoopThread<Derived>((Derived*)this);
				setSynchronizeFPS(false);
				_preinitialize(); //可以调用glfwWindowHint()，不可设置窗口参数、调用OpenGL函数，否则空指针异常
			}

			~Application()
			{

			}

			__attribute__((always_inline)) inline void _launchMainLoop()
			{
				while(is_running)
				{
					//更新
					turn(main_loop_thread);
				}
			}

			void launch()
			{
				_initialize();
				main_loop_thread->start();
				((Derived*)this)->_launchMainLoop();
				_terminate();
			}

			void exit()
			{
				is_running=false;
			}

			__attribute__((always_inline)) inline void setSynchronizeFPS(bool synchronize_fps_)
			{
				synchronize_fps=synchronize_fps_;
			}

			__attribute__((always_inline)) inline int getUpdateRate()
			{
				return main_loop_thread->getUpdateRate();
			}

			__attribute__((always_inline)) inline int getUpdateRateCount() //获取当前所在帧
			{
				return main_loop_thread->getUpdateRateCount();
			}

			__attribute__((always_inline)) inline void setUpdateRateLimit(int ur_limit)
			{
				main_loop_thread->setUpdateRateLimit(ur_limit);
			}
		};

		template<typename DerivedApp>
		class MainLoopThread:public andromeda::thread::Thread<void(),MainLoopThread<DerivedApp>>,public andromeda::thread::CoroutineLock
		{
			DefineThread
			friend class andromeda::thread::Thread<void(),MainLoopThread<DerivedApp>>;
			friend class andromeda::app::Application<DerivedApp>;

			using andromeda::thread::CoroutineLock::turn;
		protected:
			DerivedApp* app;
			FrameRate update_rate;

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
			MainLoopThread(DerivedApp *derived_app) :
					andromeda::thread::Thread<void(),MainLoopThread<DerivedApp>>(&(derived_app->is_running),andromeda::thread::ThreadWorkMode::Detach)
			{
				app=derived_app;
			}

			inline void setUpdateRateLimit(int update_fps)
			{
				update_rate.set_fps_limit(update_fps);
			}

			inline int getUpdateRate()
			{
				return update_rate.get_fps();
			}

			inline int getUpdateRateCount()
			{
				return update_rate.get_fps_count();
			}

			using andromeda::thread::Thread<void(),MainLoopThread<DerivedApp>>::start;
			using andromeda::thread::Thread<void(),MainLoopThread<DerivedApp>>::stop;
		};
	}
}

#endif//ANDROMEDA_APP_APPLICATION// @formatter:on
