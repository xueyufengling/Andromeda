#include <andromeda/app/window.h>

#include <andromeda/util/log.h>

using namespace andromeda::app;
using namespace andromeda::graphics;

namespace andromeda
{
extern void _glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
}

window::window(const char* title, int width, int height, color_rgba back_color, bool is_full_screen, GLFWmonitor* monitor) :
		width(width), height(height), is_full_screen(is_full_screen), title(title), back_color(back_color), monitor(monitor)
{
	if(!title)
		LogDebug("Window name cannot be null")
	if(!(window_id = glfwCreateWindow(width, height, title, is_full_screen ? monitor : nullptr, nullptr)))
		LogDebug("Create Window failed")
	else
		glfwSetFramebufferSizeCallback(window_id, andromeda::_glfw_framebuffer_size_callback);
}
