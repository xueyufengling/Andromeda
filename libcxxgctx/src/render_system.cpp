#include <cxxgctx/render_system.h>

using namespace cxxgctx;

int render_system::screen_width = 0;
int render_system::screen_height = 0;

render_system::render_system() :
		cams(1)
{

}

void render_system::init_properties()
{
	get_screen_resolution((int*)(&screen_width), (int*)(&screen_height));
}

void render_system::get_screen_resolution(int* width_ptr, int* height_ptr)
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	if(mode)
	{
		*width_ptr = mode->width;
		*height_ptr = mode->height;
	}
}
