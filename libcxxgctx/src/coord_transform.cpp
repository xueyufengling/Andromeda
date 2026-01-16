#include <cxxgctx/coord_transform.h>

#include <cxxgctx/gl/texture_2d.h>

using namespace cxxgctx::gl;

void cxxgctx::gui2uv(float input_x1, float input_y1, float input_x2, float input_y2, float& output_x1, float& output_y1, float& output_x2, float& output_y2, texture2d* texture)
{
	if(texture)
	{
		input_x1 /= texture->get_width();
		input_y1 /= texture->get_height();
		input_x2 /= texture->get_width();
		input_y2 /= texture->get_height();
	}
	output_x1 = input_x1;
	output_y1 = 1 - input_y2;
	output_x2 = input_x2;
	output_y2 = 1 - input_y1;
}
