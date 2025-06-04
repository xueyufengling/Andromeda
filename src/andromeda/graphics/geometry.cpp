#include <andromeda/graphics/geometry.h>

using namespace andromeda::graphics;
using namespace andromeda::math;

void Polygon::generateVertexData()
{
	geo_strategy=GeoStrategy::TRIANGLE_FAN;
	size_t point_num=vertices.length();
	size_t extra_data_len=vertex_extra_data.length()/point_num;
	for(size_t i=0;i<point_num;++i)
	{
		vertex_data.addv(vertices[i][0],vertices[i][1],0.0f);
		vertex_data.add(vertex_extra_data+i*extra_data_len,extra_data_len);
	}
}

Polygon Polygon::ptQuad(float width,float height)
{
	Polygon quad;
	float x=width/2;
	float y=height/2;
	quad.addVertex(Vector2f{-x,-y},0.0f,0.0f);
	quad.addVertex(Vector2f{-x,y},0.0f,1.0f);
	quad.addVertex(Vector2f{x,y},1.0f,1.0f);
	quad.addVertex(Vector2f{x,-y},1.0f,0.0f);
	return quad;
}
