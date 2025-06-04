#ifndef ANDROMEDA_APP_TRANSPARENTFULLWINDOWAPPLICATION
#define ANDROMEDA_APP_TRANSPARENTFULLWINDOWAPPLICATION

#include "../graphics/color_rgba.h"
#include "../graphics/internal_shaders.h"
#include "window_application.h"

// @formatter:off
/**
 * 以整个桌面为渲染背景的应用
 */

#define DefineTransparentFullWindowApplication(Derived) \
	DefineApplication(Derived)\
	friend class andromeda::app::TransparentFullWindowApplication<Derived>;\

#define ImportTransparentFullWindowApplicationAPI(Derived) \
	using andromeda::app::Application<andromeda::app::TransparentFullWindowApplication<Derived> >::launch;\
	using andromeda::app::Application<andromeda::app::TransparentFullWindowApplication<Derived> >::exit;

namespace andromeda {
	namespace app {
		template<typename Derived>
		class TransparentFullWindowApplication:public andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >
		{
			DefineApplication(TransparentFullWindowApplication<Derived>)
		public:
			TransparentFullWindowApplication(const char* window_title=nullptr,bool isFullScreen=true) :
					andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >(window_title,andromeda::app::RenderSys::screenWidth,andromeda::app::RenderSys::screenHeight,andromeda::graphics::ColorRGBA::TRANSPARENT_BLACK,isFullScreen)
			{

			}

			~TransparentFullWindowApplication()=default;

			void preinitialize()
			{
				((Derived*)this)->preinitialize();
				setWindowDecorated(false);
				setWindowFramebufferTransparent(true);
				setWindowInitiallyFocused(true);
				setWindowFramebufferResizable(false);
			}

			void initialize()
			{
				((Derived*)this)->initialize();
				setBackColor(andromeda::graphics::ColorRGBA::TRANSPARENT_BLACK);
				setWindowTransparentColor(andromeda::graphics::Pixel::TRANSPARENT_BLACK);
				setIsAlwaysOnTop(true);
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

			inline static andromeda::graphics::ShaderProgram* ptShader()
			{
				return &andromeda::graphics::ptTransparentFixShaderProgram();
			}

			using andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >::operator Window*;
			using andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >::setSynchronizeFPS;
			using andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >::getRenderFPS;
			using andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >::getRenderFPSCount; //获取当前所在帧
			using andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >::setRenderFPSLimit;
			using andromeda::app::WindowApplication<TransparentFullWindowApplication<Derived> >::setUpdateRateLimit;

			using Application<TransparentFullWindowApplication<Derived> >::getUpdateRate;
			using Application<TransparentFullWindowApplication<Derived> >::getUpdateRateCount; //获取当前所在帧

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

#endif//ANDROMEDA_APP_TRANSPARENTFULLWINDOWAPPLICATION// @formatter:on
