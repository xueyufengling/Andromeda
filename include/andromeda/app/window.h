#ifndef ANDROMEDA_APP_WINDOW
#define ANDROMEDA_APP_WINDOW

#include <opengl/glad/glad.h>
#include <opengl/GLFW/glfw3.h>
#include "../graphics/color_rgba.h"

#if defined (_WIN32) || defined (_WIN64)
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined (__linux__) || defined(__APPLE__)
#endif

#include <opengl/GLFW/glfw3native.h>

namespace andromeda {
	namespace app {
		class Window
		{
		private:
			GLFWwindow* window_id=nullptr;
			GLFWmonitor* monitor=nullptr;
			int width,height;
			bool is_fullScreen;
			bool visible=true;
			const char* title=nullptr;
			andromeda::graphics::ColorRGBA back_color;

		public:
			Window()=default; //该构造函数不会实际初始化窗口仅分配内存，需要重新调用其他构造函数以正常工作
			Window(const char* title,int width=800,int height=600,andromeda::graphics::ColorRGBA backColor_={0,0,0,0},bool isfullscreen=false,GLFWmonitor* monitor_=glfwGetPrimaryMonitor());

			inline operator GLFWwindow*()
			{
				return window_id;
			}

			inline void setBackColor(andromeda::graphics::ColorRGBA backColor_={0,0,0,0})
			{
				back_color=backColor_;
			}

			inline andromeda::graphics::ColorRGBA getBackColor()
			{
				return back_color;
			}

			inline Window& setBackColor(andromeda::graphics::ColorChannel ch,float v)
			{
				back_color.setRGBA(ch,v);
				return *this;
			}

			inline float getBackColor(andromeda::graphics::ColorChannel ch)
			{
				return back_color.getRGBA(ch);
			}

			inline int getWidth(void)
			{
				return width;
			}

			inline int getHeight(void)
			{
				return height;
			}

			inline bool isWindowFullScreen(void)
			{
				return is_fullScreen;
			}

			inline Window& setFullScreen(bool isFullScreen_)
			{
				is_fullScreen=isFullScreen_;
				glfwSetWindowMonitor(window_id,isFullScreen_?monitor:nullptr,0,0,width,height,GLFW_DONT_CARE);
				return *this;
			}

			//是否显示窗口
			inline Window& setVisible(bool visible)
			{
				this->visible=visible;
				if(visible)
					glfwShowWindow(window_id);
				else
					glfwHideWindow(window_id);
				return *this;
			}

			inline Window& setWindowed(bool windowed,int pos_x,int pos_y)
			{
				this->monitor=NULL;
				glfwSetWindowMonitor(window_id,NULL,pos_x,pos_y,width,height,0);
				return *this;
			}

			inline Window& setMonitor(GLFWmonitor* monitor)
			{
				this->monitor=monitor;
				const GLFWvidmode* mode=glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window_id,monitor,0,0,mode->width,mode->height,mode->refreshRate);
				return *this;
			}

			inline Window& setWindowSize(int width_,int height_)
			{
				width=width_;
				height=height_;
				glfwSetWindowSize(window_id,width_,height_);
				return *this;
			}

			inline Window& setIsAlwaysOnTop(bool floating=false) //设置窗口是否总是置顶
			{
				glfwSetWindowAttrib(window_id,GLFW_FLOATING,floating);
				return *this;
			}

			inline bool isAlwaysOnTop()
			{
				return glfwGetWindowAttrib(window_id,GLFW_FLOATING);
			}

			inline void setWindowFramebufferTransparent(bool transparent=false) //设置窗口渲染的背景是否透明。注意即使背景透明，事件依旧不会穿透窗口透明部分，它们会被窗口捕获
			{
				glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,transparent);
			}

			inline bool isWindowFramebufferTransparent()
			{
				return glfwGetWindowAttrib(window_id,GLFW_TRANSPARENT_FRAMEBUFFER);
			}

			inline void setWindowFramebufferResizable(bool resizable=false)
			{
				glfwWindowHint(GLFW_RESIZABLE,resizable);
			}

			inline bool isWindowFramebufferResizable()
			{
				return glfwGetWindowAttrib(window_id,GLFW_RESIZABLE);
			}

			inline void setWindowInitiallyVisible(bool init_visible=true) //窗口初始是否可见
			{
				glfwWindowHint(GLFW_VISIBLE,init_visible);
			}

			inline bool isWindowInitiallyVisible()
			{
				return glfwGetWindowAttrib(window_id,GLFW_VISIBLE);
			}

			inline void setWindowInitiallyFocused(bool init_focused=true) //窗口初始是否可见
			{
				glfwWindowHint(GLFW_FOCUSED,init_focused);
			}

			inline bool isWindowInitiallyFocused()
			{
				return glfwGetWindowAttrib(window_id,GLFW_FOCUSED);
			}

			inline void setWindowDecorated(bool decorated=true) //窗口初始是否可见
			{
				glfwWindowHint(GLFW_DECORATED,decorated);
			}

			inline bool isWindowDecorated()
			{
				return glfwGetWindowAttrib(window_id,GLFW_DECORATED);
			}

			inline float setWindowOpacity(float opacity=1) //窗口整体的透明度设置，位于0-1间
			{
				glfwSetWindowOpacity(window_id,opacity);
				return glfwGetWindowOpacity(window_id); //返回当前的窗口透明度。如果设置成功则应该返回opacity
			}

#ifdef GLFW_MOUSE_PASSTHROUGH
			inline void setWindowMouseEventPassthrough(bool passthrough) //设置窗口鼠标事件是否穿透，glfw3.4版本及以上可用，整个窗口均会鼠标事件穿透
			{
				glfwWindowHint(GLFW_MOUSE_PASSTHROUGH,passthrough);
			}

			inline bool isWindowMouseEventPassthrough(bool passthrough)
			{
				return glfwGetWindowAttrib(window_id,GLFW_MOUSE_PASSTHROUGH);
			}
#endif

#if defined (_WIN32) || defined (_WIN64)
			inline HWND handle()
			{
				return glfwGetWin32Window(window_id);
			}

			inline static void _Win_SetWindowExLayered(HWND handle)
			{
				LONG style= GetWindowLong(handle,GWL_EXSTYLE);
				style|= WS_EX_LAYERED;
				SetWindowLong(handle,GWL_EXSTYLE,style);
			}

			inline static bool _Win_SetWindowAlpha(HWND handle,andromeda::graphics::Pixel transparent_color,int rest_alpha)
			{
				return SetLayeredWindowAttributes(handle,RGB(transparent_color.r,transparent_color.g,transparent_color.b),rest_alpha,LWA_ALPHA|LWA_COLORKEY);
			}

			inline static bool _Win_SetWindowAlpha(HWND handle,andromeda::graphics::Pixel transparent_color)
			{
				return SetLayeredWindowAttributes(handle,RGB(transparent_color.r,transparent_color.g,transparent_color.b),0xFF,LWA_COLORKEY);
			}

			inline static bool _Win_SetWindowAlpha(HWND handle,int rest_alpha)
			{
				return SetLayeredWindowAttributes(handle,NULL,rest_alpha,LWA_ALPHA);
			}
#endif

			inline void setWindowTransparentColor(andromeda::graphics::Pixel transparent_color) //设置窗口透明色，透明处鼠标事件穿透，不透明的地方依然不会穿透
			{
#if defined (_WIN32) || defined (_WIN64)
				HWND hd=handle();
				_Win_SetWindowExLayered(hd);
				_Win_SetWindowAlpha(hd,transparent_color);
				return;
#elif defined (__linux__) || defined(__APPLE__)
#endif
			}
		};
	}
}

#endif//ANDROMEDA_APP_WINDOW
