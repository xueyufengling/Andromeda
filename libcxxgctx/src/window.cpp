#include <cxxgctx/window.h>

#include <map>
#include <cxxcomm/log.h>

using namespace cxxgctx;
using namespace cxxcomm;
using namespace cxxsci;

static std::map<GLFWwindow*, window*> instances;

static void cxxgctx::__resize_callback(GLFWwindow* glfw_window, int new_width, int new_height)
{
	window* pw = instances[glfw_window];
	int orig_width = pw->width;
	int orig_height = pw->height;
	pw->width = new_width;
	pw->height = new_height;
	if(pw->window_resize_callback)
		pw->window_resize_callback(*pw, orig_width, orig_height, new_width, new_height);
}

window::window(const char* title, int width, int height, color_rgba back_color, bool is_full_screen, GLFWmonitor* monitor) :
		width(width), height(height), is_full_screen(is_full_screen), title(title), back_color(back_color), monitor(monitor)
{
	if(!title)
		LogError("window name cannot be null")
	if(!(window_id = glfwCreateWindow(width, height, title, is_full_screen ? monitor : nullptr, nullptr)))
		LogError("create window failed")
	else
	{
		instances[window_id] = this;
		glfwSetFramebufferSizeCallback(window_id, __resize_callback);
	}
}
