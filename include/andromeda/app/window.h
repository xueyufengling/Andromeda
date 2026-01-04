#ifndef ANDROMEDA_APP_WINDOW
#define ANDROMEDA_APP_WINDOW

#include "../platform/platform.h"
#include "../graphics/gl_basic.h"
#include "../graphics/color_rgba.h"
#include "../media/raster_image.h"

namespace andromeda
{
namespace app
{
class window
{
private:
	GLFWwindow* window_id = nullptr;
	GLFWmonitor* monitor = nullptr;
	int width = 800, height = 600;
	bool is_full_screen = false;
	bool visible = true;
	const char* title = nullptr;
	andromeda::graphics::color_rgba back_color;

public:
	window() = default; //该构造函数不会实际初始化窗口仅分配内存，需要重新调用其他构造函数以正常工作
	window(const char* title, int width = 800, int height = 600, andromeda::graphics::color_rgba back_color = {0, 0, 0, 0}, bool is_full_screen = false, GLFWmonitor* monitor = glfwGetPrimaryMonitor());

	inline operator GLFWwindow*()
	{
		return window_id;
	}

	inline void set_icon(GLFWimage* icon)
	{
		glfwSetWindowIcon(window_id, 1, icon);
	}

	/**
	 * @brief 设置窗口图标
	 */
	inline void set_icon(andromeda::media::raster_image& icon)
	{
		set_icon((GLFWimage*)icon);
	}

	inline void set_back_color(andromeda::graphics::color_rgba back_color = {0, 0, 0, 0})
	{
		this->back_color = back_color;
	}

	inline andromeda::graphics::color_rgba get_back_color()
	{
		return back_color;
	}

	inline window& set_back_color(andromeda::graphics::color_channel ch, float v)
	{
		back_color.set_rgba(ch, v);
		return *this;
	}

	inline float get_back_color(andromeda::graphics::color_channel ch)
	{
		return back_color.get_rgba(ch);
	}

	inline int get_width(void)
	{
		return width;
	}

	inline int get_height(void)
	{
		return height;
	}

	inline bool is_window_full_screen(void)
	{
		return is_full_screen;
	}

	inline window& set_full_screen(bool is_full_screen)
	{
		this->is_full_screen = is_full_screen;
		glfwSetWindowMonitor(window_id, is_full_screen ? monitor : nullptr, 0, 0, width, height, GLFW_DONT_CARE);
		return *this;
	}

	//是否显示窗口
	inline window& set_visible(bool visible)
	{
		this->visible = visible;
		if(visible)
			glfwShowWindow(window_id);
		else
			glfwHideWindow(window_id);
		return *this;
	}

	inline window& set_windowed(bool windowed, int pos_x, int pos_y)
	{
		this->monitor = NULL;
		glfwSetWindowMonitor(window_id, NULL, pos_x, pos_y, width, height, 0);
		return *this;
	}

	inline window& set_monitor(GLFWmonitor* monitor)
	{
		this->monitor = monitor;
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window_id, monitor, 0, 0, mode->width, mode->height, mode->refresh_rate);
		return *this;
	}

	inline window& set_window_size(int width, int height)
	{
		this->width = width;
		this->height = height;
		glfwSetWindowSize(window_id, width, height);
		return *this;
	}

	inline window& set_always_on_top(bool floating = false) //设置窗口是否总是置顶
	{
		glfwSetWindowAttrib(window_id, GLFW_FLOATING, floating);
		return *this;
	}

	inline bool is_always_on_top()
	{
		return glfwGetWindowAttrib(window_id, GLFW_FLOATING);
	}

	inline void set_window_framebuffer_transparent(bool transparent = false) //设置窗口渲染的背景是否透明。注意即使背景透明，事件依旧不会穿透窗口透明部分，它们会被窗口捕获
	{
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, transparent);
	}

	inline bool is_window_framebuffer_transparent()
	{
		return glfwGetWindowAttrib(window_id, GLFW_TRANSPARENT_FRAMEBUFFER);
	}

	inline void set_window_framebuffer_resizable(bool resizable = false)
	{
		glfwWindowHint(GLFW_RESIZABLE, resizable);
	}

	inline bool is_window_framebuffer_resizable()
	{
		return glfwGetWindowAttrib(window_id, GLFW_RESIZABLE);
	}

	inline void set_window_initially_visible(bool init_visible = true) //窗口初始是否可见
	{
		glfwWindowHint(GLFW_VISIBLE, init_visible);
	}

	inline bool is_window_initially_visible()
	{
		return glfwGetWindowAttrib(window_id, GLFW_VISIBLE);
	}

	inline void set_window_initially_focused(bool init_focused = true) //窗口初始是否可见
	{
		glfwWindowHint(GLFW_FOCUSED, init_focused);
	}

	inline bool is_window_initially_focused()
	{
		return glfwGetWindowAttrib(window_id, GLFW_FOCUSED);
	}

	inline void set_window_decorated(bool decorated = true) //窗口初始是否可见
	{
		glfwWindowHint(GLFW_DECORATED, decorated);
	}

	inline bool is_window_decorated()
	{
		return glfwGetWindowAttrib(window_id, GLFW_DECORATED);
	}

	inline float set_window_opacity(float opacity = 1) //窗口整体的透明度设置，位于0-1间
	{
		glfwSetWindowOpacity(window_id, opacity);
		return glfwGetWindowOpacity(window_id); //返回当前的窗口透明度。如果设置成功则应该返回opacity
	}

#ifdef GLFW_MOUSE_PASSTHROUGH
	inline void set_window_mouse_event_passthrough(bool passthrough) //设置窗口鼠标事件是否穿透，glfw3.4版本及以上可用，整个窗口均会鼠标事件穿透
	{
		glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, passthrough);
	}

	inline bool is_window_mouse_event_passthrough(bool passthrough)
	{
		return glfwGetWindowAttrib(window_id, GLFW_MOUSE_PASSTHROUGH);
	}
#endif

#if defined(__PLATFORM_WIN__)
	inline HWND handle()
	{
		return glfwGetWin32Window(window_id);
	}

	inline static void _win_set_window_ex_layered(HWND handle)
	{
		LONG style = GetWindowLong(handle, GWL_EXSTYLE);
		style |= WS_EX_LAYERED;
		SetWindowLong(handle, GWL_EXSTYLE, style);
	}

	inline static bool _win_set_window_alpha(HWND handle, andromeda::graphics::pixel transparent_color, int rest_alpha)
	{
		return SetLayeredWindowAttributes(handle, RGB(transparent_color.r, transparent_color.g, transparent_color.b), rest_alpha, LWA_ALPHA | LWA_COLORKEY);
	}

	inline static bool _win_set_window_alpha(HWND handle, andromeda::graphics::pixel transparent_color)
	{
		return SetLayeredWindowAttributes(handle, RGB(transparent_color.r, transparent_color.g, transparent_color.b), 0xFF, LWA_COLORKEY);
	}

	inline static bool _win_set_window_alpha(HWND handle, int rest_alpha)
	{
		return SetLayeredWindowAttributes(handle, NULL, rest_alpha, LWA_ALPHA);
	}
#endif

	inline void set_window_transparent_color(andromeda::graphics::pixel transparent_color) //设置窗口透明色，透明处鼠标事件穿透，不透明的地方依然不会穿透
	{
#if defined (_WIN32) || defined (_WIN64)
		HWND hd = handle();
		_win_set_window_ex_layered(hd);
		_win_set_window_alpha(hd, transparent_color);
		return;
#elif defined (__linux__) || defined(__APPLE__)
#endif
	}
};
}
}

#endif//ANDROMEDA_APP_WINDOW
