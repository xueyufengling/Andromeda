#ifndef ANDROMEDA_APP_WINDOWAPPLICATION
#define ANDROMEDA_APP_WINDOWAPPLICATION

#include <andromeda/app/render_system.h>
#include <andromeda/common/log.h>
#include "window.h"
#include "frame_rate.h"
#include "../graphics/gl_basic.h"
#include "../graphics/color_rgba.h"
#include "../graphics/framebuffer.h"
#include "application.h"

/**
 * 带有图形和音频的应用，可用于编写客户端
 * GLFW只能在主线程使用，因此主线程负责窗口事件及渲染
 */

namespace andromeda
{
namespace app
{
template<typename Derived>
class window_application: public window, public andromeda::app::application<Derived>
{
	DefineApplication(Derived)

private:
	using andromeda::app::application<Derived>::is_running;
	using andromeda::app::application<Derived>::synchronize_fps;
	using andromeda::app::application<Derived>::app_main_loop_thread;
	frame_rate render_frame_rate; //渲染循环计数器
	andromeda::graphics::framebuffer framebuffer; //双缓冲
	int render_fps_limit = -1;
	int update_rate_limit = -1;

protected:
	using andromeda::app::application<Derived>::_initialize;
	using andromeda::app::application<Derived>::_preinitialize;
	using andromeda::app::application<Derived>::_terminate;
	using andromeda::app::application<Derived>::_update;
	using andromeda::app::application<Derived>::_render_update;
	using andromeda::app::application<Derived>::turn;

	render_system render_sys;

public:
	ImportApplicationAPI(Derived)

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
		new (this) window(window_title ? window_title : "Andromeda Application", width, height, back_color, is_full_screen, monitor); //初始化window
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
				turn(app_main_loop_thread);
			//渲染
			framebuffer.bind_this();
			framebuffer.clear_all_buffers();
			if(andromeda::traits::is_class<Derived>::result && func_exist(render_update)<void, float>::check<Derived>::result) //如果子类没有render_update()则此调用将优化掉
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
			app_main_loop_thread->set_update_rate_limit(-1);
	}

	__attribute__((always_inline)) inline void set_update_rate_limit(int ur_limit)
	{
		update_rate_limit = ur_limit;
		app_main_loop_thread->set_update_rate_limit(ur_limit);
		if(synchronize_fps)
			render_frame_rate.set_fps_limit(-1); //每个线程如果开启帧率限制，均会休眠一帧剩余的时间。防止多个线程均休眠导致浪费多帧时间，一帧中只能有一个线程休眠
	}

	using application<Derived>::get_update_rate;
	using application<Derived>::get_update_rate_count; //获取当前所在帧

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