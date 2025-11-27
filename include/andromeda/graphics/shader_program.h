#ifndef ANDROMEDA_GRAPHICS_SHADERPROGRAM
#define ANDROMEDA_GRAPHICS_SHADERPROGRAM

#include <andromeda/common/string_utils.h>
#include "../math/vector.h"
#include "../math/matrix.h"
#include "gl_basic.h"

namespace andromeda
{
namespace graphics
{

class shader_program: public gl_object<shader_program>
{
private:
	GLuint vertex_shader = 0, fragment_shader = 0;
	const char* vertex_shader_source = nullptr;
	const char* fragment_shader_source = nullptr;
	shader_program(GLuint shader_program);
	shader_program(GLuint shader_program, GLuint vertex_shader, GLuint fragment_shader);

public:
	using gl_object<shader_program>::operator GLuint;

	/**
	 * 使用该着色器，不记录上个使用的着色器
	 */
	__attribute__((always_inline)) inline static void bind(GLuint program_id)
	{
		gl_UseProgram(program_id);
	}

	__attribute__((always_inline)) inline static void invalidate(GLuint program_id)
	{
		gl_UseProgram(program_id);
	}

	inline static GLuint current_bind()
	{
		return andromeda::graphics::gl_get_integer(GL_CURRENT_PROGRAM);
	}

	shader_program(const char* vertex_shader_source, const char* fragment_shader_source);
	//编译、链接失败则不改变shader_program,vertex_shader,fragment_shader的值
	bool set_vertex_shader(const char* vertex_shader_source); //当着色器都编译完成后将自动链接，返回值为当前程序状态，true表示无错误，false表示有错误不可用（编译错误或链接错误）
	bool set_fragment_shader(const char* fragment_shader_source);
	bool set_vertex_shader(GLuint vertex_shader); //设置已经编译好的着色器，如果checkVertexShader()返回false说明着色器实际上有错，则该函数返回false。程序链接失败也返回false
	bool set_fragment_shader(GLuint fragment_shader);

	bool link(bool release_shader = true); //通常不需要调用，可以自动链接，成功则返回true。release_shader决定如果链接成功是否释放已经编译的着色器资源，默认释放资源

	/**
	 * 释放已经编译的着色器资源
	 */
	void release_shader();

	/**
	 * @brief 检查着色器是否编译成功，如果未成功则打印错误日志
	 * @return true为编译成功，false为编译失败
	 */
	static bool check_shader(GLuint shader);

	static bool check_program(GLuint shader_program);

	__attribute__((always_inline)) inline bool check_vertex_shader()
	{
		return check_shader(vertex_shader);
	}

	__attribute__((always_inline)) inline bool check_fragment_shader()
	{
		return check_shader(fragment_shader);
	}

	__attribute__((always_inline)) inline bool check_program()
	{
		return check_program(obj_id);
	}

//适用于偶尔设置变量值（glGetUniformLocation查询代价高昂避免循环调用！），设置前后不改变当前着色器程序
	void set(const char* name, const int value); //设置程序中的变量值，OpenGL不支持从着色器程序返回变量值
	void set(const char* name, const unsigned int value);
	void set(const char* name, const float value);
	void set(const char* name, int count, const int* value_arr);
	void set(const char* name, int count, const unsigned int* value_arr);
	void set(const char* name, int count, const float* value_arr);
	void set(const char* name, int count, const andromeda::math::matrix3x3f* value_arr, bool transpose = false);
	void set(const char* name, const bool value); //调用setUnsignedInt设置为0或1
	void set(const char* name, const andromeda::math::vector3f vec3);
	void set(const char* name, const andromeda::math::matrix3x3f& mat3, bool transpose = false);
	void set(const char* name, const andromeda::math::matrix3x3f* mat3, bool transpose = false);

//适用于频繁设置变量值，使用前使用use()设定着色器后才可以调用set()和setArray()
	class variable
	{
	private:
		GLuint shader_program;
		GLint var_loc;

	public:
		operator GLint()
		{
			return var_loc;
		}

		inline variable(GLuint shader_program, const char* name) :
				shader_program(shader_program)
		{
			var_loc = gl_GetUniformLocation(shader_program, name);
		}

		inline void use()
		{
			gl_UseProgram(shader_program);
		}

		inline void set(const int value)
		{
			gl_Uniform1i(var_loc, value);
		}

		inline void set(const unsigned int value)
		{
			gl_Uniform1ui(var_loc, value);
		}

		inline void set(const float value)
		{
			gl_Uniform1f(var_loc, value);
		}

		inline void set(const int count, int* value_arr)
		{
			gl_Uniform1iv(var_loc, count, value_arr);
		}

		inline void set(int count, const unsigned int* value_arr)
		{
			gl_Uniform1uiv(var_loc, count, value_arr);
		}

		inline void set(int count, const float* value_arr)
		{
			gl_Uniform1fv(var_loc, count, value_arr);
		}

		inline void set(const andromeda::math::vector3f vec3)
		{
			gl_Uniform1fv(var_loc, 3, (const GLfloat*)&vec3);
		}

		inline void set(const andromeda::math::matrix3x3f& mat3, bool transpose = false)
		{
			gl_UniformMatrix3fv(var_loc, 1, transpose, (const GLfloat*)&mat3);
		}

		inline void set(const andromeda::math::matrix3x3f* mat3, bool transpose = false)
		{
			gl_UniformMatrix3fv(var_loc, 1, transpose, (const GLfloat*)mat3);
		}
	};

	variable get_variable(const char* name)
	{
		return variable(id(), name);
	}
}
;
}
}

#endif //ANDROMEDA_GRAPHICS_SHADERPROGRAM
