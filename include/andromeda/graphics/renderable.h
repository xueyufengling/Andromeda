#ifndef ANDROMEDA_GRAPHICS_RENDERABLE
#define ANDROMEDA_GRAPHICS_RENDERABLE

#include <andromeda/common/array.h>
#include <map>
#include <string>

#include "texture_2d.h"
#include "vertex_attribute.h"
#include "shader_program.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/transform.h"

#define DIRECT_INSTANCE_CAPCITY 1024

namespace andromeda
{
namespace graphics
{
class renderable;

//同一个对象的不同实例
class renderable_instance
{
	friend class renderable;
	private:
	andromeda::math::matrix4x4f model_matrix = andromeda::math::matrix4x4f::identity(); //变换数据
	andromeda::common::array<unsigned char> instance_divisor_data; //自定义数据，以字节为单位
public:
	renderable_instance() = default;

	inline renderable_instance& scale(float scale_factor)
	{
		model_matrix = cast<andromeda::math::matrix4x4f>(model_matrix * andromeda::math::scale(scale_factor));
		return *this;
	}

	inline renderable_instance& shift(andromeda::math::vector3f shift)
	{
		model_matrix = cast<andromeda::math::matrix4x4f>(model_matrix * andromeda::math::shift(shift));
		return *this;
	}

	inline renderable_instance& rotate(float rad_x, float rad_y, float rad_z)
	{
		model_matrix = cast<andromeda::math::matrix4x4f>(model_matrix * andromeda::math::Rxyz(rad_x, rad_y, rad_z));
		return *this;
	}

	inline renderable_instance& rotate(andromeda::math::vector3f axis, float rad)
	{
		model_matrix = cast<andromeda::math::matrix4x4f>(model_matrix * andromeda::math::R(axis, rad));
		return *this;
	}

	template<typename T>
	inline T& append_divisor(T& data)
	{
		instance_divisor_data.add(&data, sizeof(T));
	}

	inline void append_divisor(void* data, size_t size)

	{
		instance_divisor_data.add((unsigned char*)data, size);
	}
};

/* 绘制策略为NONE则不绘制；NORMAL表示普通绘制，INSTANCED表示实例化绘制
 * 普通绘制只会绘制一个物体，这个物体由vertex_data和element_data定义
 * 实例化绘制将以普通绘制的物体为模板，每个实例有自己的divisor数据，这部分数据添加在vertex_data全部顶点数据的后面
 */
enum struct draw_strategy : GLuint
{
	NONE, NORMAL, INSTANCED
};

enum struct geometry_strategy : GLuint
{
	POINTS = 0, LINES, LINE_LOOP, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP
};

enum struct data_strategy : GLuint
{
	STREAM_DRAW = GL_STREAM_DRAW, STREAM_READ = GL_STREAM_READ, STREAM_COPY = GL_STREAM_COPY, STATIC_DRAW = GL_STATIC_DRAW, STATIC_READ = GL_STATIC_READ, STATIC_COPY = GL_STATIC_COPY, DYNAMIC_DRAW = GL_DYNAMIC_DRAW, DYNAMIC_READ = GL_DYNAMIC_READ, DYNAMIC_COPY = GL_DYNAMIC_COPY
};

/* 最小可渲染对象，该对象作为一个整体渲染。更复杂的物体可能由多个部件组合而成，这种情况应该使用RenderableModel
 * 只包含渲染物体本身的信息（顶点数据、着色器、纹理等），不包括位置、变换等（这些属于实例的信息，不同实例可以共享同一个Renderable对象），等价于Mesh
 */
class renderable
{
private:
	GLuint vao = 0;
	GLuint vbo = 0; //顶点数据
	GLuint ebo = 0;
	draw_strategy instance_draw_strategy = draw_strategy::NONE;
	GLuint instance_vbo = 0; //实例数据
	andromeda::common::array<renderable_instance> direct_instance_list; //不具名的实例对象，通常是创建后不需要操作的对象，也可以用数组中的索引获取对象实例
	std::map<std::string, renderable_instance> named_instance_map; //具名的实例对象
	andromeda::common::array<unsigned char> instance_divisor_data;

protected:
	vertex_attribute* vertex_attribs = nullptr;
	shader_program* shader = nullptr;

public:
	andromeda::common::array<float> vertex_data; //顶点数据的指针，采用局部坐标系
	andromeda::common::array<unsigned int> element_data; //索引数据的指针，可选
	andromeda::common::array<texture2d> textures; //索引对应不同纹理单元，默认只有一个纹理单元0。纹理均为0级纹理，minimap靠程序自动生成
	andromeda::math::matrix4x4f vertex_pretransform = andromeda::math::matrix4x4f::identity(); //加载模型后需要进行的变换
	geometry_strategy instance_geometry_strategy = geometry_strategy::TRIANGLE_STRIP; //绘制的几何形状
	data_strategy vertex_data_strategy = data_strategy::STATIC_DRAW; //原始顶点数据的数据策略，静态数据、动态改变数据或流式数据。通常都是静态STATIC_DRAW，即模型加载后顶点数据不变，而位置变换等通过实例化绘制

	renderable();

	//传入id返回对应的具名实例(如果该具名实例不存在则新建)，或传入空字符串得到一个不可被索引的不具名实例
	renderable_instance& instance(std::string id = std::string());

	inline renderable_instance operator[](int direct_idx)
	{
		return direct_instance_list[direct_idx];
	}

	inline renderable_instance operator[](std::string name)
	{
		return named_instance_map[name];
	}

	inline operator GLuint()
	{
		return vao;
	}

	inline size_t get_vertex_count()
	{
		return vertex_data.length() / vertex_attribs->get_vertex_size();
	}

	inline size_t get_element_count()
	{
		return element_data.length();
	}

	inline void use()
	{
		shader->bind_this();
		use_textures();
		glBindVertexArray(vao);
	}

	void pretransform_vertex();

	//设置顶点属性和对应的着色器程序。如果是nullptr则使用此前的顶点属性或着色器
	void set_vertex_attribute_shader(vertex_attribute* vertex_attribs = nullptr, shader_program* shader = nullptr);

	//设置绘制策略，顶点属性或着色器为nullptr的话，则使用此前的顶点属性或着色器
	void set_draw_strategy(draw_strategy strategy, vertex_attribute* vertex_attribs = nullptr, shader_program* shader = nullptr)
	{
		instance_draw_strategy = strategy;
		set_vertex_attribute_shader(vertex_attribs, shader);
	}

	__attribute__((always_inline)) inline void draw() //bufferVertexData()后才可以调用，否则报错
	{
		use();
		switch(instance_draw_strategy)
		{
		case draw_strategy::INSTANCED:
			{
			size_t instance_count = direct_instance_list.length() + named_instance_map.size();
			if(instance_count < 1)
				return;
			buffer_divisor_data(data_strategy::DYNAMIC_DRAW);
			if(ebo)
				gl_DrawElementsInstanced((GLuint)instance_geometry_strategy, get_element_count(), GL_UNSIGNED_INT, 0, instance_count);
			else
				gl_DrawArraysInstanced((GLuint)instance_geometry_strategy, 0, get_vertex_count(), instance_count);
		}
			break;
		case draw_strategy::NORMAL:
			{
			if(ebo)
				gl_DrawElements((GLuint)instance_geometry_strategy, get_element_count(), GL_UNSIGNED_INT, 0);
			else
				gl_DrawArrays((GLuint)instance_geometry_strategy, 0, get_vertex_count());
		}
			break;
		}
	}

	void load_textures(bool generate_minimap = true, bool release_image = true); //加载所有纹理到对应的纹理单元
	void use_textures(); //使用所有纹理（在不同纹理单元中），loadTextures()后才可调用

	void buffer_vertex_data(); //加载顶点数据以及索引数据（如果有的话）进OpenGL，并设置好vertex_attribs对应的顶点属性格式。只能调用一次，此后数据只可读取不能更改
	void buffer_divisor_data(data_strategy data_strategy = data_strategy::DYNAMIC_DRAW); //bufferVertexData()后调用。加载实例数据进OpenGL，如果实例每帧变化则需要每次绘制前调用。

	/**
	 * 设置顶点数据，数据将拷贝并储存
	 */
	inline renderable& set_vertex_data(float* vertex_data, size_t vertex_data_count, unsigned int* element_data = nullptr, size_t element_count = 0)
	{
		this->vertex_data.clear();
		this->vertex_data.add(vertex_data, vertex_data_count);
		this->element_data.clear();
		this->element_data.add(element_data, element_count);
		pretransform_vertex();
		return *this;
	}

	template<typename ...Textures>
	inline void add_textures(Textures ... _textures) //必须全部传入Texture2D类型的对象
	{
		textures.add(_textures...);
	}
};
}
}

#endif//ANDROMEDA_GRAPHICS_RENDERABLE