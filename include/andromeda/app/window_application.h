#ifndef ANDROMEDA_APP_WINDOWAPPLICATION
#define ANDROMEDA_APP_WINDOWAPPLICATION

#include "../common/log.h"
#include "window.h"
#include "frame_rate.h"
#include "../graphics/gl_basic.h"
#include "../graphics/color_rgba.h"
#include "../graphics/framebuffer.h"
#include "render_system.h"
#include "application.h"

/**
 * 带有图形和音频的应用，可用于编写客户端
 * GLFW只能在主线程使用，因此主线程负责窗口事件及渲染
 */
namespace andromeda
{
namespace app
{
template<typename _Derived>
class window_application: public window, public andromeda::app::application<_Derived>
{
	ConstructApplication(_Derived)

private:
	using andromeda::app::application<_Derived>::is_running;
	using andromeda::app::application<_Derived>::synchronize_fps;
	using andromeda::app::application<_Derived>::app_main_loop_thread;
	frame_rate render_frame_rate; //渲染循环计数器
	andromeda::graphics::framebuffer framebuffer; //双缓冲
	int render_fps_limit = andromeda::app::frame_rate::UNLIMITED;
	int update_rate_limit = andromeda::app::frame_rate::UNLIMITED;

	using andromeda::app::application<_Derived>::_initialize;
	using andromeda::app::application<_Derived>::_preinitialize;
	using andromeda::app::application<_Derived>::_terminate;
	using andromeda::app::application<_Derived>::_update;
	using andromeda::app::application<_Derived>::_render_update;
	using andromeda::app::application<_Derived>::swap;

protected:
	render_system render_sys;

public:
	window_application(const char* window_title = nullptr, int width = 800, int height = 600, andromeda::graphics::color_rgba back_color = {0, 0, 0, 0}, bool is_full_screen = false, GLFWmonitor* monitor = glfwGetPrimaryMonitor())
	{
		if(!andromeda::graphics::load_glfw())
		{
			LogFatal("Load GLFW failed");
			goto FAILED;
		}
		//if(!andromeda::use_openal)
		{
			LogFatal("Load OpenAL failed");
			//goto FAILED;
		}
		synchronize_fps = true; //默认开启帧率同步
		_preinitialize(); //可以调用glfwWindowHint()，不可设置窗口参数、调用OpenGL函数，否则空指针异常
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		new (this) window(window_title ? window_title : "", width, height, back_color, is_full_screen, monitor); //初始化window
		glfwMakeContextCurrent(*this);
		if(!andromeda::graphics::load_gl()) //GLAD的加载要在glfwMakeContextCurrent()之后进行
		{
			LogFatal("Load OpenGL failed");
			goto FAILED;
		}
		new (&framebuffer) andromeda::graphics::framebuffer(width, height, back_color);
		framebuffer.alloc();
		set_back_color(back_color);
		return;
		FAILED:
		LogFatal("Application initialize failed");
		return;
	}

	~window_application()
	{
		glfwDestroyWindow(*this);
	}

	__attribute__((always_inline)) inline operator window*()
	{
		return this;
	}

	using window::operator GLFWwindow*;

	__attribute__((always_inline)) inline void _launch_main_loop()
	{
		render_frame_rate.init();
		while(is_running && (!glfwWindowShouldClose(*this)))
		{
			//输入处理
			glfwPollEvents();
			//开启同步帧率则优先执行更新函数
			if(synchronize_fps)
				swap(app_main_loop_thread);
			//渲染
			framebuffer.bind_this();
			framebuffer.clear_all_buffers();
			if(is_class<_Derived>::result && exist_memb_with_type(_Derived, void(float), render_update)::result) //如果子类没有render_update()则此调用将优化掉
				_render_update(render_frame_rate.get_tpf());
			framebuffer.blit_to_screen();
			glfwSwapBuffers(*this);
			render_frame_rate.calc();
		}
	}

	__attribute__((always_inline)) inline void set_synchronize_fps(bool synchronize_fps_)
	{
		synchronize_fps = synchronize_fps_;
		render_frame_rate.set_fps_limit(render_fps_limit);
		if(synchronize_fps_)
			app_main_loop_thread->set_update_rate_limit(andromeda::app::frame_rate::UNLIMITED); //开启了帧率同步，则以渲染帧率去同步更新率
		else
			app_main_loop_thread->set_update_rate_limit(update_rate_limit);
	}

	__attribute__((always_inline)) inline int get_render_fps()
	{
		return render_frame_rate.get_fps();
	}

	__attribute__((always_inline)) inline int get_render_fps_count() //获取当前所在帧
	{
		return render_frame_rate.get_fps_count();
	}

	__attribute__((always_inline)) inline void set_render_fps_limit(int fps_limit)
	{
		render_fps_limit = fps_limit;
		render_frame_rate.set_fps_limit(fps_limit);
		if(synchronize_fps)
			app_main_loop_thread->set_update_rate_limit(andromeda::app::frame_rate::UNLIMITED);
	}

	__attribute__((always_inline)) inline void set_update_rate_limit(int ur_limit)
	{
		update_rate_limit = ur_limit;
		app_main_loop_thread->set_update_rate_limit(ur_limit);
		if(synchronize_fps)
			render_frame_rate.set_fps_limit(andromeda::app::frame_rate::UNLIMITED);
	}

	using application<_Derived>::get_update_rate;
	using application<_Derived>::get_update_rate_count; //获取当前所在帧

	using window::set_back_color;
	using window::get_back_color;
	using window::get_height;
	using window::get_width;

	using window::is_window_full_screen;
	using window::set_full_screen;
	using window::set_visible;
	using window::set_windowed;
	using window::set_monitor;
	using window::set_window_size;
	using window::set_always_on_top;
	using window::set_window_framebuffer_transparent;
	using window::is_window_framebuffer_transparent;
	using window::set_window_framebuffer_resizable;

	using window::is_window_framebuffer_resizable;
	using window::set_window_initially_visible;
	using window::is_window_initially_visible;
	using window::set_window_initially_focused;
	using window::is_window_initially_focused;
	using window::set_window_decorated;
	using window::is_window_decorated;
	using window::set_window_opacity;

#ifdef GLFW_MOUSE_PASSTHROUGH
	using window::set_window_mouse_event_passthrough;
	using window::is_window_mouse_event_passthrough;

#endif

	using window::set_window_transparent_color;
};
}
}

#endif//ANDROMEDA_APP_WINDOWAPPLICATION