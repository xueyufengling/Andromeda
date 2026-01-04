#include <andromeda/graphics/shader_program.h>
#include <andromeda/log/log.h>

using namespace andromeda::graphics;
using namespace andromeda::math;

shader_program::shader_program(GLuint shader_program, GLuint vertex_shader, GLuint fragment_shader) :
		gl_object(shader_program), vertex_shader(vertex_shader), fragment_shader(fragment_shader)
{
	if((!check_program()) && check_vertex_shader() && check_fragment_shader()) //shader_program无效，但所有着色器都有效则链接着色器程序
		link();
}

shader_program::shader_program(GLuint shader_program) :
		gl_object(shader_program)
{
}

shader_program::shader_program(const char* vertex_shader_source, const char* fragment_shader_source)
{
	set_vertex_shader(vertex_shader_source);
	set_fragment_shader(fragment_shader_source);
}

bool shader_program::check_shader(GLuint shader)
{
	if(!shader)
		return false;
	int success;
	gl_GetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char info[512]{0};
		gl_GetShaderInfoLog(shader, 512, GL_ZERO_INDEX, info);
		LogError("gl shader compile failed:", info, "\nshader id:", shader)
	}
	return (bool)success;
}

bool shader_program::check_program(GLuint shader_program)
{
	if(!shader_program)
		return false;
	int success;
	gl_GetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success)
	{
		char info[512];
		gl_GetProgramInfoLog(shader_program, 512, GL_ZERO_INDEX, info);
		LogError("shader program link failed:", info, "\nprogram id:", shader_program)
	}
	return (bool)success;
}

bool shader_program::set_vertex_shader(const char* vertex_shader_source)
{
	this->vertex_shader_source = vertex_shader_source;
	GLuint new_vertex_shader = gl_CreateShader(GL_VERTEX_SHADER);
	gl_ShaderSource(new_vertex_shader, 1, &vertex_shader_source, GL_ZERO_INDEX);
	gl_CompileShader(new_vertex_shader);
	return set_vertex_shader(new_vertex_shader);
}

bool shader_program::set_fragment_shader(const char* fragment_shader_source)
{
	this->fragment_shader_source = fragment_shader_source;
	GLuint new_fragment_shader = gl_CreateShader(GL_FRAGMENT_SHADER);
	gl_ShaderSource(new_fragment_shader, 1, &fragment_shader_source, GL_ZERO_INDEX);
	gl_CompileShader(new_fragment_shader);
	return set_fragment_shader(new_fragment_shader);
}

bool shader_program::set_vertex_shader(GLuint vertex_shader)
{
	bool shader_compiled = check_vertex_shader();
	if(shader_compiled)
	{
		gl_DeleteShader(this->vertex_shader);
		this->vertex_shader = vertex_shader;
	}
	else
	{
		gl_DeleteShader(vertex_shader);
		return false;
	}
	if(fragment_shader && check_fragment_shader()) //如果所有着色器均编译成功则链接
		return link();
	return shader_compiled;
}

bool shader_program::set_fragment_shader(GLuint fragment_shader)
{
	bool shader_compiled = check_fragment_shader();
	if(shader_compiled)
	{
		gl_DeleteShader(this->fragment_shader);
		this->fragment_shader = fragment_shader;
	}
	else
	{
		gl_DeleteShader(fragment_shader);
		return false;
	}
	if(vertex_shader && check_vertex_shader()) //如果所有着色器均编译成功则链接
		return link();
	return shader_compiled;
}

bool shader_program::link(bool release_shader)
{
	GLuint old_program = obj_id;
	if(check_vertex_shader() && check_fragment_shader()) //所有着色器都编译成功后才执行链接操作
	{
		obj_id = glCreateProgram();
		gl_AttachShader(obj_id, vertex_shader);
		gl_AttachShader(obj_id, fragment_shader);
		gl_LinkProgram(obj_id);
		bool link_state = check_program();
		if(link_state)
		{
			gl_DeleteProgram(old_program);
			if(release_shader)
				this->release_shader();
			return true;
		}
		else
		{
			gl_DeleteProgram(obj_id);
			obj_id = old_program;
			return false;
		}
	}
	return false;
}

void shader_program::release_shader()
{
	gl_DeleteShader(vertex_shader);
	vertex_shader = 0;
	gl_DeleteShader(fragment_shader);
	fragment_shader = 0;
}

void shader_program::set(const char* name, const int value)
{
	gl_Uniform1i(gl_GetUniformLocation(obj_id, name), value);
}

void shader_program::set(const char* name, const unsigned int value)
{
	gl_Uniform1ui(gl_GetUniformLocation(obj_id, name), value);
}

void shader_program::set(const char* name, const bool value)
{
	gl_Uniform1ui(gl_GetUniformLocation(obj_id, name), value ? 1 : 0);
}

void shader_program::set(const char* name, const vector3f vec3)
{
	gl_Uniform1fv(gl_GetUniformLocation(obj_id, name), 3, (const GLfloat* )&vec3);
}

void shader_program::set(const char* name, const float value)
{
	gl_Uniform1f(gl_GetUniformLocation(obj_id, name), value);
}

void shader_program::set(const char* name, int count, const int* value_arr)
{
	gl_Uniform1iv(gl_GetUniformLocation(obj_id, name), count, value_arr);
}

void shader_program::set(const char* name, int count, const unsigned int* value_arr)
{
	gl_Uniform1uiv(gl_GetUniformLocation(obj_id, name), count, value_arr);
}

void shader_program::set(const char* name, int count, const float* value_arr)
{
	gl_Uniform1fv(gl_GetUniformLocation(obj_id, name), count, value_arr);
}

void shader_program::set(const char* name, int count, const andromeda::math::matrix3x3f* value_arr, bool transpose)
{
	gl_UniformMatrix4fv(gl_GetUniformLocation(obj_id, name), count, transpose, (const GLfloat* )value_arr);
}

void shader_program::set(const char* name, const matrix3x3f& mat3, bool transpose)
{
	gl_UniformMatrix3fv(gl_GetUniformLocation(obj_id, name), 1, transpose, (const GLfloat* )&mat3);
}

void shader_program::set(const char* name, const matrix3x3f* mat3, bool transpose)
{
	gl_UniformMatrix3fv(gl_GetUniformLocation(obj_id, name), 1, transpose, (const GLfloat* )mat3);
}
