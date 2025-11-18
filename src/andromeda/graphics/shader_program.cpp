#include <andromeda/graphics/shader_program.h>

#include <andromeda/util/log.h>

using namespace andromeda::graphics;
using namespace andromeda::math;

shader_program::shader_program(GLuint shader_program, GLuint vertex_shader, GLuint fragment_shader) :
		gl_component(shader_program), vertex_shader(vertex_shader), fragment_shader(fragment_shader)
{
	if((!check_shader_program(false)) && check_vertex_shader(false) && check_fragment_shader(false)) //shader_program无效，但所有着色器都有效则链接着色器程序
		link();
}

shader_program::shader_program(GLuint shader_program) :
		gl_component(shader_program)
{
}

shader_program::shader_program(const char* vertex_shader_source, const char* fragment_shader_source)
{
	set_vertex_shader(vertex_shader_source);
	set_fragment_shader(fragment_shader_source);
}

bool shader_program::check_shader(const char* shader_name, GLuint shader, bool print_log)
{
	if(!shader)
		return false;
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success && print_log)
	{
		char info[512]{0};
		glGetShaderInfoLog(shader, 512, NULL, info);
		LogDebug(shader_name, "\ncompile failed:", info, "\nShader ID:", shader)
	}
	return (bool)success;
}

bool shader_program::check_shader_program(GLuint shader_program, bool print_log)
{
	int success;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success && print_log)
	{
		char info[512];
		glGetProgramInfoLog(shader_program, 512, NULL, info);
		LogDebug("Shader Program link failed:", info, "\nID:", shader_program)
	}
	return (bool)success;
}

bool shader_program::set_vertex_shader(const char* vertex_shader_source)
{
	this->vertex_shader_source = vertex_shader_source;
	GLuint new_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(new_vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(new_vertex_shader);
	return set_vertex_shader(new_vertex_shader);
}

bool shader_program::set_fragment_shader(const char* fragment_shader_source)
{
	this->fragment_shader_source = fragment_shader_source;
	GLuint new_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(new_fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(new_fragment_shader);
	return set_fragment_shader(new_fragment_shader);
}

bool shader_program::set_vertex_shader(GLuint vertex_shader)
{
	bool shader_compiled = check_vertex_shader(vertex_shader);
	if(shader_compiled)
	{
		glDeleteShader(this->vertex_shader);
		this->vertex_shader = vertex_shader;
	}
	else
	{
		glDeleteShader(vertex_shader);
		return false;
	}
	if(fragment_shader && check_fragment_shader()) //如果所有着色器均编译成功则链接
		return link();
	return shader_compiled;
}

bool shader_program::set_fragment_shader(GLuint fragment_shader)
{
	bool shader_compiled = check_fragment_shader(fragment_shader);
	if(shader_compiled)
	{
		glDeleteShader(this->fragment_shader);
		this->fragment_shader = fragment_shader;
	}
	else
	{
		glDeleteShader(fragment_shader);
		return false;
	}
	if(vertex_shader && check_vertex_shader()) //如果所有着色器均编译成功则链接
		return link();
	return shader_compiled;
}

bool shader_program::link(bool release_shader)
{
	GLuint old_program = gl_id;
	if(check_vertex_shader() && check_fragment_shader()) //所有着色器都编译成功后才执行链接操作
	{
		gl_id = glCreateProgram();
		glAttachShader(gl_id, vertex_shader);
		glAttachShader(gl_id, fragment_shader);
		glLinkProgram(gl_id);
		bool link_state = check_shader_program();
		if(link_state)
		{
			glDeleteProgram(old_program);
			if(release_shader)
				this->release_shader();
			return true;
		}
		else
		{
			glDeleteProgram(gl_id);
			gl_id = old_program;
			return false;
		}
	}
	return false;
}

void shader_program::release_shader()
{
	glDeleteShader(vertex_shader);
	vertex_shader = 0;
	glDeleteShader(fragment_shader);
	fragment_shader = 0;
}

void shader_program::set(const char* name, const int value)
{
	glUniform1i(glGetUniformLocation(gl_id, name), value);
}

void shader_program::set(const char* name, const unsigned int value)
{
	glUniform1ui(glGetUniformLocation(gl_id, name), value);
}

void shader_program::set(const char* name, const bool value)
{
	glUniform1ui(glGetUniformLocation(gl_id, name), value ? 1 : 0);
}

void shader_program::set(const char* name, const vector3f vec3)
{
	glUniform1fv(glGetUniformLocation(gl_id, name), 3, (const GLfloat*)&vec3);
}

void shader_program::set(const char* name, const float value)
{
	glUniform1f(glGetUniformLocation(gl_id, name), value);
}

void shader_program::set(const char* name, int count, const int* value_arr)
{
	glUniform1iv(glGetUniformLocation(gl_id, name), count, value_arr);
}

void shader_program::set(const char* name, int count, const unsigned int* value_arr)
{
	glUniform1uiv(glGetUniformLocation(gl_id, name), count, value_arr);
}

void shader_program::set(const char* name, int count, const float* value_arr)
{
	glUniform1fv(glGetUniformLocation(gl_id, name), count, value_arr);
}

void shader_program::set(const char* name, int count, const andromeda::math::matrix3x3f* value_arr, bool transpose)
{
	glUniformMatrix4fv(glGetUniformLocation(gl_id, name), count, transpose, (const GLfloat*)value_arr);
}

void shader_program::set(const char* name, const matrix3x3f& mat3, bool transpose)
{
	glUniformMatrix3fv(glGetUniformLocation(gl_id, name), 1, transpose, (const GLfloat*)&mat3);
}

void shader_program::set(const char* name, const matrix3x3f* mat3, bool transpose)
{
	glUniformMatrix3fv(glGetUniformLocation(gl_id, name), 1, transpose, (const GLfloat*)mat3);
}
