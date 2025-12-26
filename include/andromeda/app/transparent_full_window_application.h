#ifndef ANDROMEDA_APP_TRANSPARENTFULLWINDOWAPPLICATION
#define ANDROMEDA_APP_TRANSPARENTFULLWINDOWAPPLICATION

#include "../graphics/color_rgba.h"
#include "../graphics/internal_shaders.h"
#include "window_application.h"

#define ConstructTransparentFullWindowApplication(Derived) \
	ConstructApplication(Derived)\
	friend class andromeda::app::transparent_full_window_application<Derived>;\

namespace andromeda
{
namespace app
{
/**
 * 以整个桌面为渲染背景的应用
 */
template<typename Derived>
class transparent_full_window_application: public andromeda::app::window_application<transparent_full_window_application<Derived> >
{
	ConstructApplication(Derived)

public:
	transparent_full_window_application(const char* window_title = nullptr, bool isFullScreen = true) :
			andromeda::app::window_application<transparent_full_window_application<Derived> >(window_title, andromeda::app::render_system::screen_width, andromeda::app::render_system::screen_height, andromeda::graphics::color_rgba::TRANSPARENT_BLACK, isFullScreen)
	{

	}

	~transparent_full_window_application() = default;

	void preinitialize()
	{
		((Derived*)this)->preinitialize();
		set_window_decorated(false);
		set_window_framebuffer_transparent(true);
		set_window_initially_focused(true);
		set_window_framebuffer_resizable(false);
	}

	void initialize()
	{
		((Derived*)this)->initialize();
		set_back_color(andromeda::graphics::color_rgba::TRANSPARENT_BLACK);
		set_window_transparent_color(andromeda::graphics::pixel::TRANSPARENT_BLACK);
		set_always_on_top(true);
	}

	void terminate()
	{
		((Derived*)this)->terminate();
	}

	void update(float tpf)
	{
		((Derived*)this)->update(tpf);
	}

	void render_update(float render_tpf)
	{
		((Derived*)this)->render_update(render_tpf);
	}

	using andromeda::app::window_application<transparent_full_window_application<Derived> >::operator window*;
	using andromeda::app::window_application<transparent_full_window_application<Derived> >::setSynchronizeFPS;
	using andromeda::app::window_application<transparent_full_window_application<Derived> >::getRenderFPS;
	using andromeda::app::window_application<transparent_full_window_application<Derived> >::getRenderFPSCount; //获取当前所在帧
	using andromeda::app::window_application<transparent_full_window_application<Derived> >::setRenderFPSLimit;
	using andromeda::app::window_application<transparent_full_window_application<Derived> >::setUpdateRateLimit;

	using application<transparent_full_window_application<Derived> >::getUpdateRate;
	using application<transparent_full_window_application<Derived> >::getUpdateRateCount; //获取当前所在帧

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

#endif//ANDROMEDA_APP_TRANSPARENTFULLWINDOWAPPLICATION
