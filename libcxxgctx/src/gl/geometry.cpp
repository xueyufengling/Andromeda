#include <cxxgctx/gl/geometry.h>

using namespace cxxgctx::gl;
using namespace cxxsci;

void polygon::generate_vertex_data()
{
	instance_geometry_strategy = geometry_strategy::TRIANGLE_FAN;
	size_t point_num = vertices.length();
	size_t extra_data_len = vertex_extra_data.length() / point_num;
	for(size_t i = 0; i < point_num; ++i)
	{
		vertex_data.add(vertices[i][0], vertices[i][1], 0.0f);
		vertex_data.add(vertex_extra_data + i * extra_data_len, extra_data_len);
	}
}

polygon polygon::ptQuad(float width, float height)
{
	polygon quad;
	float x = width / 2;
	float y = height / 2;
	quad.add_vertex(vector2f{-x, -y}, 0.0f, 0.0f);
	quad.add_vertex(vector2f{-x, y}, 0.0f, 1.0f);
	quad.add_vertex(vector2f{x, y}, 1.0f, 1.0f);
	quad.add_vertex(vector2f{x, -y}, 1.0f, 0.0f);
	return quad;
}
