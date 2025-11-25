#ifndef ANDROMEDA_APP_RENDERSYS
#define ANDROMEDA_APP_RENDERSYS

#include <andromeda/common/array.h>
#include <map>

#include "window.h"
#include "../graphics/camera.h"

namespace andromeda
{
namespace app
{
class render_system
{
private:
	andromeda::common::array<andromeda::graphics::camera> cams; //需要计算渲染的cam

public:
	render_system();

	static int screen_width;
	static int screen_height;

	static void init_properties();

	static void get_screen_resolution(int* width_ptr, int* heightptr);
};
}
}

#endif//ANDROMEDA_APP_RENDERSYS
