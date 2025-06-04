#ifndef ANDROMEDA_GRAPHICS_RENDERABLE
#define ANDROMEDA_GRAPHICS_RENDERABLE

#include <map>
#include <string>

#include <opengl/glad/glad.h>

#include "../util/array_list.h"
#include "texture_2d.h"
#include "vertex_attribute.h"
#include "shader_program.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/transform.h"

#define DIRECT_INSTANCE_CAPCITY 1024

namespace andromeda {
	namespace graphics {
		class Renderable;

		//同一个对象的不同实例
		class RenderableInstance
		{
			friend class Renderable;
		private:
			andromeda::math::Matrix4x4f model_matrix=andromeda::math::Matrix4x4f::identity(); //变换数据
			andromeda::util::ArrayList<unsigned char> instance_divisor_data; //自定义数据，以字节为单位
		public:
			RenderableInstance()=default;

			inline RenderableInstance& scale(float scale_factor)
			{
				model_matrix=andromeda::traits::cast<andromeda::math::Matrix4x4f>(model_matrix*andromeda::math::scale(scale_factor));
				return *this;
			}

			inline RenderableInstance& shift(andromeda::math::Vector3f shift)
			{
				model_matrix=andromeda::traits::cast<andromeda::math::Matrix4x4f>(model_matrix*andromeda::math::shift(shift));
				return *this;
			}

			inline RenderableInstance& rotate(float rad_x,float rad_y,float rad_z)
			{
				model_matrix=andromeda::traits::cast<andromeda::math::Matrix4x4f>(model_matrix*andromeda::math::Rxyz(rad_x,rad_y,rad_z));
				return *this;
			}

			inline RenderableInstance& rotate(andromeda::math::Vector3f axis,float rad)
			{
				model_matrix=andromeda::traits::cast<andromeda::math::Matrix4x4f>(model_matrix*andromeda::math::R(axis,rad));
				return *this;
			}

			template<typename T>
			inline T& appendDivisorData(T& data)
			{
				instance_divisor_data.add(&data,sizeof(T));
			}

			inline void appendDivisorData(void* data,size_t size)

			{
				instance_divisor_data.add((unsigned char*)data,size);
			}
		};

		/* 绘制策略为NONE则不绘制；NORMAL表示普通绘制，INSTANCED表示实例化绘制
		 * 普通绘制只会绘制一个物体，这个物体由vertex_data和element_data定义
		 * 实例化绘制将以普通绘制的物体为模板，每个实例有自己的divisor数据，这部分数据添加在vertex_data全部顶点数据的后面
		 */
		enum struct DrawStrategy : GLuint
		{
			NONE, NORMAL, INSTANCED
		};

		enum struct GeoStrategy : GLuint
		{
			POINTS=0, LINES, LINE_LOOP, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP
		};

		enum struct DataStrategy : GLuint
		{
			STREAM_DRAW=GL_STREAM_DRAW, STREAM_READ=GL_STREAM_READ, STREAM_COPY=GL_STREAM_COPY, STATIC_DRAW=GL_STATIC_DRAW, STATIC_READ=GL_STATIC_READ, STATIC_COPY=GL_STATIC_COPY, DYNAMIC_DRAW=GL_DYNAMIC_DRAW, DYNAMIC_READ=GL_DYNAMIC_READ, DYNAMIC_COPY=GL_DYNAMIC_COPY
		};

		/* 最小可渲染对象，该对象作为一个整体渲染。更复杂的物体可能由多个部件组合而成，这种情况应该使用RenderableModel
		 * 只包含渲染物体本身的信息（顶点数据、着色器、纹理等），不包括位置、变换等（这些属于实例的信息，不同实例可以共享同一个Renderable对象），等价于Mesh
		 */
		class Renderable
		{
		private:
			GLuint vao=0;
			GLuint vbo=0; //顶点数据
			GLuint ebo=0;
			DrawStrategy draw_strategy=DrawStrategy::NONE;
			GLuint instance_vbo=0; //实例数据
			andromeda::util::ArrayList<RenderableInstance> direct_instance_list; //不具名的实例对象，通常是创建后不需要操作的对象，也可以用数组中的索引获取对象实例
			std::map<std::string,RenderableInstance> named_instance_map; //具名的实例对象
			andromeda::util::ArrayList<unsigned char> instance_divisor_data;

		protected:
			VertexAttribute* vertex_attribs=nullptr;
			ShaderProgram* shader=nullptr;

		public:
			andromeda::util::ArrayList<float> vertex_data; //顶点数据的指针，采用局部坐标系
			andromeda::util::ArrayList<unsigned int> element_data; //索引数据的指针，可选
			andromeda::util::ArrayList<Texture2D> textures; //索引对应不同纹理单元，默认只有一个纹理单元0。纹理均为0级纹理，minimap靠程序自动生成
			andromeda::math::Matrix4x4f vertex_pretransform=andromeda::math::Matrix4x4f::identity(); //加载模型后需要进行的变换
			GeoStrategy geo_strategy=GeoStrategy::TRIANGLE_STRIP; //绘制的几何形状
			DataStrategy vertex_data_strategy=DataStrategy::STATIC_DRAW; //原始顶点数据的数据策略，静态数据、动态改变数据或流式数据。通常都是静态STATIC_DRAW，即模型加载后顶点数据不变，而位置变换等通过实例化绘制

			Renderable();

			//传入id返回对应的具名实例(如果该具名实例不存在则新建)，或传入空字符串得到一个不可被索引的不具名实例
			RenderableInstance& instance(std::string id=std::string());

			inline RenderableInstance operator[](int direct_idx)
			{
				return direct_instance_list[direct_idx];
			}

			inline RenderableInstance operator[](std::string name)
			{
				return named_instance_map[name];
			}

			inline operator GLuint()
			{
				return vao;
			}

			inline size_t getVertexCount()
			{
				return vertex_data.length()/vertex_attribs->getVertexSize();
			}

			inline size_t getElementCount()
			{
				return element_data.length();
			}

			inline void use()
			{
				shader->use();
				useTextures();
				glBindVertexArray(vao);
			}

			void pretransformVertex();

			//设置顶点属性和对应的着色器程序。如果是nullptr则使用此前的顶点属性或着色器
			void setVertexAttributeShader(VertexAttribute* vertex_attribs=nullptr,ShaderProgram* shader=nullptr);

			//设置绘制策略，顶点属性或着色器为nullptr的话，则使用此前的顶点属性或着色器
			void setDrawStrategy(DrawStrategy strategy,VertexAttribute* vertex_attribs=nullptr,ShaderProgram* shader=nullptr)
			{
				draw_strategy=strategy;
				setVertexAttributeShader(vertex_attribs,shader);
			}

			__attribute__((always_inline)) inline void draw() //bufferVertexData()后才可以调用，否则报错
			{
				use();
				switch(draw_strategy)
				{
				case DrawStrategy::INSTANCED:
				{
					size_t instance_count=direct_instance_list.length()+named_instance_map.size();
					if(instance_count<1)
						return;
					bufferDivisorData(DataStrategy::DYNAMIC_DRAW);
					if(ebo)
						glDrawElementsInstanced((GLuint)geo_strategy,getElementCount(),GL_UNSIGNED_INT,0,instance_count);
					else
						glDrawArraysInstanced((GLuint)geo_strategy,0,getVertexCount(),instance_count);
				}
					break;
				case DrawStrategy::NORMAL:
				{
					if(ebo)
						glDrawElements((GLuint)geo_strategy,getElementCount(),GL_UNSIGNED_INT,0);
					else
						glDrawArrays((GLuint)geo_strategy,0,getVertexCount());
				}
					break;
				}
			}

			void loadTextures(bool generate_minimap=true,bool release_image=true); //加载所有纹理到对应的纹理单元
			void useTextures(); //使用所有纹理（在不同纹理单元中），loadTextures()后才可调用

			void bufferVertexData(); //加载顶点数据以及索引数据（如果有的话）进OpenGL，并设置好vertex_attribs对应的顶点属性格式。只能调用一次，此后数据只可读取不能更改
			void bufferDivisorData(DataStrategy data_strategy=DataStrategy::DYNAMIC_DRAW); //bufferVertexData()后调用。加载实例数据进OpenGL，如果实例每帧变化则需要每次绘制前调用。

			/**
			 * 设置顶点数据，数据将拷贝并储存
			 */
			inline Renderable& setVertexData(float* vertex_data,size_t vertex_data_count,unsigned int* element_data=nullptr,size_t element_count=0)
			{
				this->vertex_data.clear();
				this->vertex_data.add(vertex_data,vertex_data_count);
				this->element_data.clear();
				this->element_data.add(element_data,element_count);
				pretransformVertex();
				return *this;
			}

			template<typename ...Textures>
			inline void addTextures(Textures ... _textures) //必须全部传入Texture2D类型的对象
			{
				textures.add(_textures...);
			}
		};
	}
}

#endif//ANDROMEDA_GRAPHICS_RENDERABLE