#ifndef _CXXGCTX_RENDERSYS
#define _CXXGCTX_RENDERSYS

#include <map>

#include <cxxcomm/array.h>
#include <cxxgctx/window.h>
#include <cxxgctx/camera.h>

namespace cxxgctx
{
class render_system
{
private:
	cxxcomm::array<cxxgctx::camera> cams; //需要计算渲染的cam

public:
	render_system();

	static int screen_width;
	static int screen_height;

	static void init_properties();

	static void get_screen_resolution(int* width_ptr, int* heightptr);
};

}

#endif//_CXXGCTX_RENDERSYS
