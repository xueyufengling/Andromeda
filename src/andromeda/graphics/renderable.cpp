#include <andromeda/graphics/renderable.h>

#include <andromeda/graphics/internal_shaders.h>

using namespace andromeda::graphics;
using namespace andromeda::math;
using namespace andromeda::util;

Renderable::Renderable() :
		direct_instance_list(DIRECT_INSTANCE_CAPCITY), vertex_data(1024), element_data(1024), textures(Texture2D::getMaxTextureUnit()), instance_divisor_data(1024)
{
	setDrawStrategy(DrawStrategy::NORMAL,&VertexAttribute::getDefaultPTVertexAttributes(),&ptDefaultShaderProgram());
}

RenderableInstance& Renderable::instance(std::string id)
{
	if(id.empty())
		return direct_instance_list.add_placement_new();
	else
		return named_instance_map[id];
}

void Renderable::loadTextures(bool generate_minimap,bool release_image)
{
	for(int i=0;i<textures.length();++i)
		textures[i].load(i,0,generate_minimap,release_image);
}

void Renderable::useTextures()
{
	for(int i=0;i<textures.length();++i)
		textures[i].use(i);
}

void Renderable::pretransformVertex()
{
	size_t vertex_count=getVertexCount();
	for(size_t idx=0;idx<vertex_count;++idx)
	{
		int vertex_position_offset=vertex_attribs->getVertexAttributeInfo(VERTEX_ATTRIB_NAME_POSITION).getOffset();
		Vector3f& position=*(Vector3f*)(vertex_data+idx*vertex_attribs->getVertexSize()+vertex_position_offset);
		position=vertex_pretransform*position;
	}
}

void Renderable::bufferVertexData()
{
	size_t vertex_count=getVertexCount();
	if((!vertex_data)||(!vertex_count)) //如果没有顶点数据则直接返回
		return;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,vertex_count*vertex_attribs->getVertexSize(),vertex_data,(GLuint)vertex_data_strategy);
	if(element_data&&getElementCount())
	{
		glGenBuffers(1,&ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*getElementCount(),element_data,(GLuint)vertex_data_strategy);
	}
	vertex_attribs->load(vao);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void Renderable::bufferDivisorData(DataStrategy data_strategy)
{
	if(draw_strategy!=DrawStrategy::INSTANCED) //如果不是实例模式则直接返回
		return;
	if(!instance_vbo)
		glGenBuffers(1,&instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER,instance_vbo);
	instance_divisor_data.clear();
	for(size_t direct_idx=0;direct_idx<direct_instance_list.length();++direct_idx)
	{
		RenderableInstance& instance=direct_instance_list[direct_idx];
		instance_divisor_data.add(instance.instance_divisor_data,instance.instance_divisor_data.length());
	}
	for(auto iter=named_instance_map.begin();iter!=named_instance_map.end();iter++)
	{
		RenderableInstance& instance=(*iter).second;
		instance_divisor_data.add(instance.instance_divisor_data,instance.instance_divisor_data.length());
	}
	glBufferData(GL_ARRAY_BUFFER,instance_divisor_data.length(),instance_divisor_data,(GLuint)data_strategy); //实例数据是需要经常变化的
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Renderable::setVertexAttributeShader(VertexAttribute *vertex_attribs,ShaderProgram *shader)
{
	if(vertex_attribs)
		this->vertex_attribs=vertex_attribs;
	if(shader)
		this->shader=shader;
}
