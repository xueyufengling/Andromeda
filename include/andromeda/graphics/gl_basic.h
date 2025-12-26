#ifndef ANDROMEDA_GRAPHICS_GLBASIC
#define ANDROMEDA_GRAPHICS_GLBASIC

#if defined (_WIN32) || defined (_WIN64)
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined (__linux__) || defined(__APPLE__)
#endif

extern "C"
{
#include <glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
}

#include "../common/bindable_object.h"
#include "../common/lib_call.h"

LibCallAll(glCall, GLenum, glGetError, GL_NO_ERROR)

inline GLenum gl_GetIntegerv(GLenum pname, GLint* data)
{
	return glCallRet(glGetIntegerv, pname, data);
}

inline GLenum gl_Flush(void)
{
	return glCallRet(glFlush);
}

inline GLenum gl_BindFramebuffer(GLenum target, GLuint framebuffer)
{
	return glCallRet(glBindFramebuffer, target, framebuffer);
}

inline GLenum gl_ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	return glCallRet(glClearColor, red, green, blue, alpha);
}

inline GLenum gl_Clear(GLbitfield mask)
{
	return glCallRet(glClear, mask);
}

inline GLenum gl_GenFramebuffers(GLsizei n, GLuint* ids)
{
	return glCallRet(glGenFramebuffers, n, ids);
}

inline GLenum gl_GenTextures(GLsizei n, GLuint* textures)
{
	return glCallRet(glGenTextures, n, textures);
}

inline GLenum gl_BindTexture(GLenum target, GLuint texture)
{
	return glCallRet(glBindTexture, target, texture);
}

inline GLenum gl_TexImage2D(GLenum target,
		GLint level,
		GLint internalformat,
		GLsizei width,
		GLsizei height,
		GLint border,
		GLenum format,
		GLenum type,
		const void* data)
{
	return glCallRet(glTexImage2D, target,
			level,
			internalformat,
			width,
			height,
			border,
			format,
			type,
			data);
}

inline GLenum gl_TexParameteri(GLenum target, GLenum pname, GLint param)
{
	return glCallRet(glTexParameteri, target, pname, param);
}

inline GLenum gl_FramebufferTexture2D(GLenum target,
		GLenum attachment,
		GLenum textarget,
		GLuint texture,
		GLint level)
{
	return glCallRet(glFramebufferTexture2D, target,
			attachment,
			textarget,
			texture,
			level);
}

inline GLenum gl_GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	return glCallRet(glGenRenderbuffers, n, renderbuffers);
}

inline GLenum gl_BindRenderbuffer(GLenum target, GLuint renderbuffer)
{
	return glCallRet(glBindRenderbuffer, target, renderbuffer);
}

inline GLenum gl_RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	return glCallRet(glRenderbufferStorage, target, internalformat, width, height);
}

inline GLenum gl_FramebufferRenderbuffer(GLenum target,
		GLenum attachment,
		GLenum renderbuffertarget,
		GLuint renderbuffer)
{
	return glCallRet(glFramebufferRenderbuffer, target, attachment, renderbuffertarget, renderbuffer);
}

inline GLenum gl_GenBuffers(GLsizei n, GLuint* buffers)
{
	return glCallRet(glGenBuffers, n, buffers);
}

inline GLenum gl_GenVertexArrays(GLsizei n, GLuint* arrays)
{
	return glCallRet(glGenVertexArrays, n, arrays);
}

inline GLenum gl_CheckFramebufferStatus(GLenum target)
{
	return glCallRet(glCheckFramebufferStatus, target);
}

inline GLenum gl_DeleteFramebuffers(GLsizei n, GLuint* framebuffers)
{
	return glCallRet(glDeleteFramebuffers, n, framebuffers);
}

inline GLenum gl_DeleteTextures(GLsizei n, const GLuint* textures)
{
	return glCallRet(glDeleteTextures, n, textures);
}

inline GLenum gl_DeleteRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	return glCallRet(glDeleteRenderbuffers, n, renderbuffers);
}

inline GLenum gl_GetFramebufferAttachmentParameteriv(GLenum target,
		GLenum attachment,
		GLenum pname,
		GLint* params)
{
	return glCallRet(glGetFramebufferAttachmentParameteriv, target, attachment, pname, params);
}

inline GLenum gl_CopyTexSubImage2D(GLenum target,
		GLint level,
		GLint xoffset,
		GLint yoffset,
		GLint x,
		GLint y,
		GLsizei width,
		GLsizei height)
{
	return glCallRet(glCopyTexSubImage2D, target,
			level,
			xoffset,
			yoffset,
			x,
			y,
			width,
			height);
}

inline GLenum gl_Enable(GLenum cap)
{
	return glCallRet(glEnable, cap);
}

inline GLenum gl_Disable(GLenum cap)
{
	return glCallRet(glDisable, cap);
}

inline GLenum gl_BindVertexArray(GLuint array)
{
	return glCallRet(glBindVertexArray, array);
}

inline GLenum gl_BindBuffer(GLenum target, GLuint buffer)
{
	return glCallRet(glBindBuffer, target, buffer);
}

inline GLenum gl_BufferData(GLenum target,
		GLsizeiptr size,
		const void* data,
		GLenum usage)
{
	return glCallRet(glBufferData, target, size, data, usage);
}

inline GLenum gl_DrawElements(GLenum mode,
		GLsizei count,
		GLenum type,
		const void* indices)
{
	return glCallRet(glDrawElements, mode, count, type, indices);
}

inline GLenum gl_DrawElementsInstanced(GLenum mode,
		GLsizei count,
		GLenum type,
		const void* indices,
		GLsizei instancecount)
{
	return glCallRet(glDrawElementsInstanced, mode,
			count,
			type,
			indices,
			instancecount);
}

inline GLenum gl_DrawArraysInstanced(GLenum mode,
		GLint first,
		GLsizei count,
		GLsizei instancecount)
{
	return glCallRet(glDrawArraysInstanced, mode,
			first,
			count,
			instancecount);
}

inline GLenum gl_DrawArrays(GLenum mode,
		GLint first,
		GLsizei count)
{
	return glCallRet(glDrawArrays, mode,
			first,
			count);
}

inline GLenum gl_UseProgram(GLuint program)
{
	return glCallRet(glUseProgram, program);
}

inline GLint gl_GetUniformLocation(GLuint program, const GLchar* name)
{
	return glCallRet(glGetUniformLocation, program, name);
}

inline GLenum gl_Uniform1i(GLint location, GLint v0)
{
	return glCallRet(glUniform1i, location, v0);
}

inline GLenum gl_Uniform1ui(GLint location, GLuint v0)
{
	return glCallRet(glUniform1ui, location, v0);
}

inline GLenum gl_Uniform1f(GLint location, GLfloat v0)
{
	return glCallRet(glUniform1f, location, v0);
}

inline GLenum gl_Uniform1iv(GLint location,
		GLsizei count,
		const GLint* value)
{
	return glCallRet(glUniform1iv, location,
			count,
			value);
}

inline GLenum gl_Uniform1uiv(GLint location,
		GLsizei count,
		const GLuint* value)
{
	return glCallRet(glUniform1uiv, location,
			count,
			value);
}

inline GLenum gl_Uniform1fv(GLint location,
		GLsizei count,
		const GLfloat* value)
{
	return glCallRet(glUniform1fv, location,
			count,
			value);
}

inline GLenum gl_UniformMatrix3fv(GLint location,
		GLsizei count,
		GLboolean transpose,
		const GLfloat* value)
{
	return glCallRet(glUniformMatrix3fv, location,
			count,
			transpose,
			value);
}

inline GLenum gl_UniformMatrix4fv(GLint location,
		GLsizei count,
		GLboolean transpose,
		const GLfloat* value)
{
	return glCallRet(glUniformMatrix4fv, location,
			count,
			transpose,
			value);
}

inline GLenum gl_GetShaderiv(GLuint shader,
		GLenum pname,
		GLint* params)
{
	return glCallRet(glGetShaderiv, shader,
			pname,
			params);
}

inline GLenum gl_GetShaderInfoLog(GLuint shader,
		GLsizei maxLength,
		GLsizei* length,
		GLchar* infoLog)
{
	return glCallRet(glGetShaderInfoLog, shader,
			maxLength,
			length,
			infoLog);
}

inline GLenum gl_GetProgramiv(GLuint program,
		GLenum pname,
		GLint* params)
{
	return glCallRet(glGetProgramiv, program,
			pname,
			params);
}

inline GLenum gl_GetProgramInfoLog(GLuint program,
		GLsizei maxLength,
		GLsizei* length,
		GLchar* infoLog)
{
	return glCallRet(glGetProgramInfoLog, program,
			maxLength,
			length,
			infoLog);
}

inline GLuint gl_CreateShader(GLenum shaderType)
{
	return glCallRet(glCreateShader, shaderType);
}

inline GLenum gl_ShaderSource(GLuint shader,
		GLsizei count,
		const GLchar** string,
		const GLint* length)
{
	return glCallRet(glShaderSource, shader,
			count,
			string,
			length);
}

inline GLenum gl_CompileShader(GLuint shader)
{
	return glCallRet(glCompileShader, shader);
}

inline GLenum gl_DeleteShader(GLuint shader)
{
	return glCallRet(glDeleteShader, shader);
}

inline GLenum gl_AttachShader(GLuint program, GLuint shader)
{
	return glCallRet(glAttachShader, program, shader);
}

inline GLenum gl_LinkProgram(GLuint program)
{
	return glCallRet(glLinkProgram, program);
}

inline GLenum gl_DeleteProgram(GLuint program)
{
	return glCallRet(glDeleteProgram, program);
}

inline GLenum gl_ActiveTexture(GLenum texture)
{
	return glCallRet(glActiveTexture, texture);
}

inline GLenum gl_GenerateMipmap(GLenum target)
{
	return glCallRet(glGenerateMipmap, target);
}

inline GLenum gl_VertexAttribPointer(GLuint index,
		GLint size,
		GLenum type,
		GLboolean normalized,
		GLsizei stride,
		const void* pointer)
{
	return glCallRet(glVertexAttribPointer, index,
			size,
			type,
			normalized,
			stride,
			pointer);
}

inline GLenum gl_EnableVertexAttribArray(GLuint index)
{
	return glCallRet(glEnableVertexAttribArray, index);
}

inline GLenum gl_VertexAttribDivisor(GLuint index, GLuint divisor)
{
	return glCallRet(glVertexAttribDivisor, index, divisor);
}
inline GLenum gl_Viewport(GLint x,
		GLint y,
		GLsizei width,
		GLsizei height)
{
	return glCallRet(glViewport, x,
			y,
			width,
			height);
}

namespace andromeda
{
namespace graphics
{
extern void _glfw_error_print(int err_code, const char* description);
extern void _glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);

extern bool load_gl();
extern bool load_glfw(int major = 3, int minor = 3, GLFWerrorfun callback = andromeda::graphics::_glfw_error_print);
extern void term_glfw();

extern size_t sizeof_gl_type(GLint type); //获取OpenGL的数据类型大小

/*通过字符串缩写获取OpenGL的类型，从字符串开头进行匹配，一旦成功则不论后面字符串的内容均直接返回类型
 * b，ub，2b，3b，4b，s，us，i，ui，f，d
 */
extern GLuint gl_type(const char* str);

/**
 * 清空错误列表
 */
extern void gl_clear_error();

/**
 * 返回错误列表的下一个错误
 */
extern GLenum gl_next_error();

/**
 * 查询一个GL状态值，例如查询当前绑定的ID
 */
__attribute__((always_inline)) inline GLuint gl_get_integer(GLuint param)
{
	GLuint result = 0;
	gl_GetIntegerv(param, (GLint*)&result);
	return result;
}

template<typename Derived>
class gl_object: public andromeda::common::bindable_object<GLuint, Derived>
{
public:
	gl_object() = default;

	gl_object(GLuint existed_id) :
			andromeda::common::bindable_object<GLuint, Derived>(existed_id)
	{

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
	POINTS = GL_POINTS,
	LINES = GL_LINES,
	LINE_LOOP = GL_LINE_LOOP,
	LINE_STRIP = GL_LINE_STRIP,
	TRIANGLES = GL_TRIANGLES,
	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	TRIANGLE_FAN = GL_TRIANGLE_FAN,

#ifdef GL_QUADS
	QUADS = GL_QUADS,
#endif
#ifdef GL_QUAD_STRIP
	QUAD_STRIP = GL_QUAD_STRIP,
#endif
};

enum struct data_strategy : GLuint
{
	STREAM_DRAW = GL_STREAM_DRAW,
	STREAM_READ = GL_STREAM_READ,
	STREAM_COPY = GL_STREAM_COPY,
	STATIC_DRAW = GL_STATIC_DRAW,
	STATIC_READ = GL_STATIC_READ,
	STATIC_COPY = GL_STATIC_COPY,
	DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
	DYNAMIC_READ = GL_DYNAMIC_READ,
	DYNAMIC_COPY = GL_DYNAMIC_COPY
};

}
}

#endif//ANDROMEDA_GRAPHICS_GLBASIC
