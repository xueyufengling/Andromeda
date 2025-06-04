#ifndef ANDROMEDA_GRAPHICS_VERTEXATTRIBUTE
#define ANDROMEDA_GRAPHICS_VERTEXATTRIBUTE

#include <opengl/glad/glad.h>
#include "../util/array_list.h"
#include "../util/gl_utils.h"

#define VERTEX_ATTRIB_NAME_POSITION "position"
#define VERTEX_ATTRIB_NAME_COLOR "color"
#define VERTEX_ATTRIB_NAME_TEXCOORD "texture_coord"
#define VERTEX_ATTRIB_NAME_MODEL_MATRIX "model_matrix"

namespace andromeda {
	namespace graphics {
		class VertexAttributeInfo //属性的信息
		{
			friend class VertexAttribute;
			friend class andromeda::util::ArrayList<VertexAttributeInfo>;
		private:
			GLint index=-1;
			const char* name=nullptr;
			GLint type=GL_NONE;
			size_t type_size=0;
			bool normalized=false;
			int num=0;
			size_t offset=0; //在单个顶点中的位移(因为顶点各个属性长度可以不一样，故统一用字节数表示位移)
			size_t divisor=0;//几个实例共享数据

			VertexAttributeInfo()=default;
			VertexAttributeInfo(const char* info);

			VertexAttributeInfo(GLint index,const char* name,GLint type,bool normalized,int num,size_t offset,size_t divisor) :
					index(index), name(name), type(type), normalized(normalized), num(num), offset(offset), divisor(divisor)
			{
				type_size=andromeda::util::getGLTypeSize(type);
			}

		public:
			inline GLint getIndex()
			{
				return index;
			}

			inline GLint getType()
			{
				return type;
			}

			inline bool getNormalized()
			{
				return normalized;
			}

			inline int getNum()
			{
				return num;
			}

			inline size_t getOffset()
			{
				return offset;
			}

			inline const char* getName()
			{
				return name;
			}

			inline size_t getDivisor()
			{
				return divisor;
			}
		};

		class VertexAttribute
		{
		private:
			//不储存OpenGL的VAO对象，VAO对象与渲染对象是一一对应的，本类只储存顶点属性格式
			size_t vertex_size=0;
			andromeda::util::ArrayList<VertexAttributeInfo> attribs;
		public:
			VertexAttribute()=default;

			VertexAttribute(const char* attrib_info)
			{
				setAttribute(attrib_info);
			}

			/*
			 * 用于指定顶点格式，用法"顶点属性名称1.0:3f,顶点属性名称2.1:4f/1,顶点属性名称3.2:2f/0...."
			 * 顶点属性名称便于记忆，不可包含'.'和':'。顺序表示顶点属性序号，从0开始，例如顶点名称1为属性0，顶点名称2为属性1...
			 * 属性每次后面的'.'后面为该属性的OpenGL索引，如果不存在则默认为当前属性的位置索引（从0开始，最前面的属性为0，依次为1、2、3...）
			 * 建议要么为每个名称指定一个自定义的索引，要么就省略'.'及后面的索引，全部不指定以应用从0开始按顺序的索引
			 * n表示normalized，不加则表示不规范化。后面加数字表示数据个数（必须是1-4），最后是类型名称，类型名称定义如下
			 * b，ub，s，us，i，ui，f，d，2b，3b，4b除顶点名称其余应严格小写，例如n3f表示归一化3维向量
			 * '/'为可选的配置，如果有，则后面的数字视为divisor。即实例化渲染时多少个实例共享一份属性数据，默认为0
			 * 向量直接以nf表示，如果是n*n矩阵，则分解为n个n维列向量，一个列向量为一个顶点属性
			 */
			void setAttribute(const char* attrib_str);

			__attribute__((always_inline)) inline void load(GLuint vao) //在glBindBuffer()后、glDraw*()前调用，用于把顶点属性格式装载进指定的VAO中
			{
				glBindVertexArray(vao);
				for(size_t i=0;i<attribs.length();++i)
				{
					VertexAttributeInfo& attrib=attribs[i];
					glVertexAttribPointer(attrib.index,attrib.num,attrib.type,attrib.normalized,vertex_size,(void*)(attrib.offset));
					glEnableVertexAttribArray(attrib.index);
					if(attrib.divisor)
						glVertexAttribDivisor(attrib.index,attrib.divisor);
				}
				glBindVertexArray(0);
			}

			VertexAttributeInfo getVertexAttributeInfo(const char* attrib_name);

			inline VertexAttributeInfo getVertexAttributeInfo(int index)
			{
				return attribs[index];
			}

			inline size_t getVertexSize()
			{
				return vertex_size;
			}

			//默认的位置坐标、顶点颜色、纹理坐标的顶点属性
			static inline VertexAttribute& getDefaultPCTVertexAttributes()
			{
				static VertexAttribute default_pct_vertex_attributes=VertexAttribute(VERTEX_ATTRIB_NAME_POSITION ":3f," VERTEX_ATTRIB_NAME_COLOR ":4f," VERTEX_ATTRIB_NAME_TEXCOORD ":2f");
				return default_pct_vertex_attributes;
			}

			//默认的位置坐标、顶点颜色、纹理坐标、实例变换的顶点属性
			static inline VertexAttribute& getDefaultPCTIVertexAttributes()
			{
				static VertexAttribute default_pct_vertex_attributes=VertexAttribute(VERTEX_ATTRIB_NAME_POSITION ":3f," VERTEX_ATTRIB_NAME_COLOR ":4f," VERTEX_ATTRIB_NAME_TEXCOORD ":2f," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c1:4f/1," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c2:4f/1," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c3:4f/1," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c4:4f/1");
				return default_pct_vertex_attributes;
			}

			//默认的位置坐标、纹理坐标的顶点属性
			static inline VertexAttribute& getDefaultPTVertexAttributes()
			{
				static VertexAttribute default_pt_vertex_attributes=VertexAttribute(VERTEX_ATTRIB_NAME_POSITION ":3f," VERTEX_ATTRIB_NAME_TEXCOORD ":2f");
				return default_pt_vertex_attributes;
			}

			//默认的位置坐标、纹理坐标、实例变换的顶点属性
			static inline VertexAttribute& getDefaultPTIVertexAttributes()
			{
				static VertexAttribute default_pt_vertex_attributes=VertexAttribute(VERTEX_ATTRIB_NAME_POSITION ":3f," VERTEX_ATTRIB_NAME_TEXCOORD ":2f," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c1:4f/1," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c2:4f/1," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c3:4f/1," VERTEX_ATTRIB_NAME_MODEL_MATRIX "_c4:4f/1");
				return default_pt_vertex_attributes;
			}
		};
	}
}

#endif//ANDROMEDA_GRAPHICS_VERTEXATTRIBUTE
