#ifndef ANDROMEDA_GRAPHICS_GEOMETRY
#define ANDROMEDA_GRAPHICS_GEOMETRY

#include "renderable.h"

namespace andromeda
{
namespace graphics
{
//多边形默认深度z坐标为0，点在xy平面上
class polygon : public renderable
{
private:
	using renderable::shader;
	using renderable::vertex_attribs;
	using renderable::instance_geometry_strategy;
	andromeda::util::array_list<andromeda::math::vector2f> vertices;
	andromeda::util::array_list<float> vertex_extra_data;

public:
	using renderable::vertex_data;
	using renderable::element_data;
	using renderable::textures;

	template<typename ...Ts>
	__attribute__((always_inline)) inline void add_vertex(andromeda::math::vector2f pos, Ts ... extra_data)
	{
		vertices.addv(pos);
		vertex_extra_data.addv(extra_data...);
	}

	//生成顶点数据
	void generate_vertex_data();

	static polygon ptQuad(float width, float height);
};
}
}

#endif//ANDROMEDA_GRAPHICS_GEOMETRY
