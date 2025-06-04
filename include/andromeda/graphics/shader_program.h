#ifndef ANDROMEDA_GRAPHICS_SHADERPROGRAM
#define ANDROMEDA_GRAPHICS_SHADERPROGRAM

#include <opengl/glad/glad.h>
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../util/string_utils.h"

namespace andromeda {
	namespace graphics {
		class ShaderProgram
		{
		private:
			GLuint shader_program=0,vertex_shader=0,fragment_shader=0;
			const char* vertex_shader_source=nullptr;
			const char* fragment_shader_source=nullptr;
			ShaderProgram(GLuint shader_program);
			ShaderProgram(GLuint shader_program,GLuint vertex_shader,GLuint fragment_shader);
			public:
			operator GLuint()
			{
				return shader_program;
			}

			ShaderProgram(const char* vertex_shader_source,const char* fragment_shader_source);
			//编译、链接失败则不改变shader_program,vertex_shader,fragment_shader的值
			bool setVertexShader(const char* vertex_shader_source); //当着色器都编译完成后将自动链接，返回值为当前程序状态，true表示无错误，false表示有错误不可用（编译错误或链接错误）
			bool setFragmentShader(const char* fragment_shader_source);
			bool setVertexShader(GLuint vertex_shader); //设置已经编译好的着色器，如果checkVertexShader()返回false说明着色器实际上有错，则该函数返回false。程序链接失败也返回false
			bool setFragmentShader(GLuint fragment_shader);
			bool linkProgram(bool release_shader=true); //通常不需要调用，可以自动链接，成功则返回true。release_shader决定如果链接成功是否释放已经编译的着色器资源，默认释放资源
			GLuint use_ret(); //使用该着色器并返回上一个使用的着色器
			void releaseShader(); //释放已经编译的着色器资源

			__attribute__((always_inline)) inline void use() //使用该着色器
			{
				glUseProgram(shader_program);
			}

			static bool checkShader(const char* shader_name,GLuint shader,bool print_log=true);

			static bool checkVertexShader(ShaderProgram& program,bool print_log=true) //成功则返回true。print_log决定如果出错是否打印错误信息方便调试
			{
				return checkShader(andromeda::util::str_join("Vertex Shader:\n",program.vertex_shader_source),program.vertex_shader,print_log);
			}

			static bool checkVertexShader(GLuint shader,bool print_log=true)
			{
				return checkShader(andromeda::util::str_join("Vertex Shader:\n",shader),shader,print_log);
			}

			static bool checkFragmentShader(ShaderProgram& program,bool print_log=true) //成功则返回true
			{
				return checkShader(andromeda::util::str_join("Fragment Shader:\n",program.fragment_shader_source),program.fragment_shader,print_log);
			}

			static bool checkFragmentShader(GLuint shader,bool print_log=true)
			{
				return checkShader(andromeda::util::str_join("Fragment Shader:\n",shader),shader,print_log);
			}

			static bool checkShaderProgram(GLuint shader_program,bool print_log=true); //成功则返回true

			__attribute__((always_inline)) inline bool checkVertexShader(bool print_log=true) //成功则返回true。print_log决定如果出错是否打印错误信息方便调试
			{
				return checkVertexShader(*this,print_log);
			}

			__attribute__((always_inline)) inline bool checkFragmentShader(bool print_log=true) //成功则返回true
			{
				return checkFragmentShader(*this,print_log);
			}

			__attribute__((always_inline)) inline bool checkShaderProgram(bool print_log=true) //成功则返回true
			{
				return checkShaderProgram(shader_program,print_log);
			}

			//适用于偶尔设置变量值（glGetUniformLocation查询代价高昂避免循环调用！），设置前后不改变当前着色器程序
			void setInt(const char* name,int value); //设置程序中的变量值，OpenGL不支持从着色器程序返回变量值
			void setUnsignedInt(const char* name,unsigned int value);
			void setFloat(const char* name,float value);
			void setIntArray(const char* name,int count,int* value_arr);
			void setUnsignedIntArray(const char* name,int count,unsigned int* value_arr);
			void setFloatArray(const char* name,int count,float* value_arr);
			void setMatrix4x4fArray(const char* name,int count,bool transpose,const float* value);
			void setBool(const char* name,bool value); //调用setUnsignedInt设置为0或1
			void setVector3f(const char* name,andromeda::math::Vector3f vec3);
			void setMatrix3x3f(const char* name,andromeda::math::Matrix3x3f& mat3,bool transpose=false);
			void setMatrix3x3f(const char* name,andromeda::math::Matrix3x3f* mat3,bool transpose=false);

			//适用于频繁设置变量值，使用前使用use()设定着色器后才可以调用set()和setArray()
			class Variable
			{
			private:
				GLuint shader_program;
				GLint var_loc;
				public:
				operator GLint()
				{
					return var_loc;
				}

				inline Variable(GLuint shader_program,const char* name) :
						shader_program(shader_program)
				{
					var_loc=glGetUniformLocation(shader_program,name);
				}

				inline void use()
				{
					glUseProgram(shader_program);
				}

				inline void set(int value)
				{
					glUniform1i(var_loc,value);
				}

				inline void set(unsigned int value)
				{
					glUniform1ui(var_loc,value);
				}

				inline void set(float value)
				{
					glUniform1f(var_loc,value);
				}

				inline void setArray(int count,int* value_arr)
				{
					glUniform1iv(var_loc,count,value_arr);
				}

				inline void setArray(int count,unsigned int* value_arr)
				{
					glUniform1uiv(var_loc,count,value_arr);
				}

				inline void setArray(int count,float* value_arr)
				{
					glUniform1fv(var_loc,count,value_arr);
				}

				inline void setVector(andromeda::math::Vector3f vec3)
				{
					glUniform1fv(var_loc,3,(const GLfloat*)&vec3);
				}

				inline void setMatrix(andromeda::math::Matrix3x3f& mat3,bool transpose=false)
				{
					glUniformMatrix3fv(var_loc,1,transpose,(const GLfloat*)&mat3);
				}

				inline void setMatrix(andromeda::math::Matrix3x3f* mat3,bool transpose=false)
				{
					glUniformMatrix3fv(var_loc,1,transpose,(const GLfloat*)mat3);
				}
			};

			Variable getVariable(const char* name)
			{
				return Variable(shader_program,name);
			}
		};
	}
}

#endif //ANDROMEDA_GRAPHICS_SHADERPROGRAM
