#ifndef ANDROMEDA_APP_WINDOWAPPLICATION
#define ANDROMEDA_APP_WINDOWAPPLICATION

#include <opengl/glad/glad.h>
#include <opengl/GLFW/glfw3.h>
#include "window.h"
#include "frame_rate.h"
#include "../graphics/color_rgba.h"
#include "../graphics/framebuffer.h"
#include "../util/log.h"
#include "application.h"
#include "render_sys.h"

/**
 * 带有图形和音频的应用，可用于编写客户端
 * GLFW只能在主线程使用，因此主线程负责窗口事件及渲染
 */

namespace andromeda {
	extern bool use_opengl;
	extern bool use_openal;

	bool _load_opengl();

	namespace app {
		template<typename Derived>
		class WindowApplication:public Window,public andromeda::app::Application<Derived>
		{
			DefineApplication(Derived)
		private:
			using andromeda::app::Application<Derived>::is_running;
			using andromeda::app::Application<Derived>::synchronize_fps;
			using andromeda::app::Application<Derived>::main_loop_thread;
			FrameRate render_frame_rate; //渲染循环计数器
			andromeda::graphics::Framebuffer framebuffer; //双缓冲
			int render_fps_limit=-1;
			int update_rate_limit=-1;

		protected:
			using andromeda::app::Application<Derived>::_initialize;
			using andromeda::app::Application<Derived>::_preinitialize;
			using andromeda::app::Application<Derived>::_terminate;
			using andromeda::app::Application<Derived>::_update;
			using andromeda::app::Application<Derived>::_render_update;
			using andromeda::app::Application<Derived>::turn;

			RenderSys render_sys;

		public:
			ImportApplicationAPI(Derived)

			WindowApplication(const char* window_title=nullptr,int width=800,int height=600,andromeda::graphics::ColorRGBA backColor_={0,0,0,0},bool isfullscreen=false,GLFWmonitor* monitor_=glfwGetPrimaryMonitor())
			{
				bool init_app=true; //如果需要的库没有加载，则不初始化该类，无法使用该类
				if(!andromeda::use_opengl)
				{
					LOG_ERROR("OpenGL is not used. Please set andromeda::use_opengl to true.")
					init_app=false;
				}
				if(!andromeda::use_openal)
				{
					LOG_ERROR("OpenAL is not used. Please set andromeda::use_openal to true.")
					init_app=false;
				}
				if(!init_app)
				{
					LOG_ERROR("Application not initialized.")
					return;
				}
				synchronize_fps=true; //默认开启帧率同步
				_preinitialize(); //可以调用glfwWindowHint()，不可设置窗口参数、调用OpenGL函数，否则空指针异常
				glfwWindowHint(GLFW_DOUBLEBUFFER,GLFW_TRUE);
				new (this) Window(window_title?window_title:"Andromeda Application",width,height,backColor_,isfullscreen,monitor_); //初始化window
				glfwMakeContextCurrent(*this);
				_load_opengl(); //GLAD的加载要在glfwMakeContextCurrent()之后进行
				new (&framebuffer) andromeda::graphics::Framebuffer(width,height,backColor_);
				framebuffer.alloc();
				setBackColor(backColor_);
			}

			~WindowApplication()
			{
				glfwDestroyWindow(*this);
			}

			__attribute__((always_inline)) inline operator Window*()
			{
				return this;
			}

			using Window::operator GLFWwindow*;

			__attribute__((always_inline)) inline void _launchMainLoop()
			{
				render_frame_rate.init();
				while(is_running&&(!glfwWindowShouldClose(*this)))
				{
					//输入处理
					glfwPollEvents();
					//开启同步帧率则优先执行更新函数
					if(synchronize_fps)
						turn(main_loop_thread);
					//渲染
					framebuffer.use();
					framebuffer.clearAll();
					if(andromeda::traits::is_class<Derived>::result&&has_func(render_update)<void,float>::check<Derived>::result) //如果子类没有render_update()则此调用将优化掉
						_render_update(render_frame_rate.get_tpf());
					framebuffer.blitToScreen();
					glfwSwapBuffers(*this);
					render_frame_rate.calc();
				}
			}

			__attribute__((always_inline)) inline void setSynchronizeFPS(bool synchronize_fps_)
			{
				synchronize_fps=synchronize_fps_;
				render_frame_rate.set_fps_limit(render_fps_limit);
				main_loop_thread->setUpdateRateLimit(update_rate_limit);
			}

			__attribute__((always_inline)) inline int getRenderFPS()
			{
				return render_frame_rate.get_fps();
			}

			__attribute__((always_inline)) inline int getRenderFPSCount() //获取当前所在帧
			{
				return render_frame_rate.get_fps_count();
			}

			__attribute__((always_inline)) inline void setRenderFPSLimit(int fps_limit)
			{
				render_fps_limit=fps_limit;
				render_frame_rate.set_fps_limit(fps_limit);
				if(synchronize_fps)
					main_loop_thread->setUpdateRateLimit(-1);
			}

			__attribute__((always_inline)) inline void setUpdateRateLimit(int ur_limit)
			{
				update_rate_limit=ur_limit;
				main_loop_thread->setUpdateRateLimit(ur_limit);
				if(synchronize_fps)
					render_frame_rate.set_fps_limit(-1); //每个线程如果开启帧率限制，均会休眠一帧剩余的时间。防止多个线程均休眠导致浪费多帧时间，一帧中只能有一个线程休眠
			}

			using Application<Derived>::getUpdateRate;
			using Application<Derived>::getUpdateRateCount; //获取当前所在帧

			using Window::setBackColor;
			using Window::getBackColor;
			using Window::getHeight;
			using Window::getWidth;

			using Window::isWindowFullScreen;
			using Window::setFullScreen;
			using Window::setVisible;
			using Window::setWindowed;
			using Window::setMonitor;
			using Window::setWindowSize;
			using Window::setIsAlwaysOnTop;
			using Window::setWindowFramebufferTransparent;
			using Window::isWindowFramebufferTransparent;
			using Window::setWindowFramebufferResizable;

			using Window::isWindowFramebufferResizable;
			using Window::setWindowInitiallyVisible;
			using Window::isWindowInitiallyVisible;
			using Window::setWindowInitiallyFocused;
			using Window::isWindowInitiallyFocused;
			using Window::setWindowDecorated;
			using Window::isWindowDecorated;
			using Window::setWindowOpacity;
#ifdef GLFW_MOUSE_PASSTHROUGH
			using Window::setWindowMouseEventPassthrough;
			using Window::isWindowMouseEventPassthrough;
#endif
			using Window::setWindowTransparentColor;
		};
	}
}

#endif//ANDROMEDA_APP_WINDOWAPPLICATION