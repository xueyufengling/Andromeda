#ifndef _CXXGCTX_GL_GLBASIC
#define _CXXGCTX_GL_GLBASIC

#include <cxxllo/platform.h>
#include <cxxcomm/bindable_object.h>
#include <cxxcomm/clib_call.h>

#if defined(__PLATFORM_WIN__)
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__PLATFORM_UNIX__)
#endif

extern "C"
{
#include "glad.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
}

/**
 * @brief OpenGL中的指针形式零索引
 */
#define GL_ZERO_INDEX ((GLsizei*)0)

#define __gl_error_code__(ret) glGetError()

decl_clibcall(gl, GLenum, __gl_error_code__, GL_NO_ERROR)

/**
 * @brief 包装调用库函数的宏，日志中指示调用发生的源码实际位置
 */
#define gl_call(callable, ...) clibcall(gl, cxxcomm::log_level::LOG_DEBUG_ERROR, __log_source__, callable, ##__VA_ARGS__)

#define gl_GetIntegerv(pname, data) gl_call(glGetIntegerv, pname, data)

#define gl_Flush() gl_call(glFlush)

#define gl_BindFramebuffer(target, framebuffer) gl_call(glBindFramebuffer, target, framebuffer);

#define gl_ClearColor(red, green, blue, alpha) gl_call(glClearColor, red, green, blue, alpha)

#define gl_Clear(mask) gl_call(glClear, mask)

#define gl_GenFramebuffers(n, ids) gl_call(glGenFramebuffers, n, ids)

#define gl_GenTextures(n, textures) gl_call(glGenTextures, n, textures)

#define gl_BindTexture(target, texture) gl_call(glBindTexture, target, texture)

#define gl_TexImage2D(target, level, internalformat, width, height, border, format, type, data) gl_call(glTexImage2D, target, level, internalformat, width, height, border, format, type, data)

#define gl_TexParameteri(target, pname, param) gl_call(glTexParameteri, target, pname, param)

#define gl_FramebufferTexture2D(target, attachment, textarget, texture, level) gl_call(glFramebufferTexture2D, target, attachment, textarget, texture, level)

#define gl_GenRenderbuffers(n, renderbuffers) gl_call(glGenRenderbuffers, n, renderbuffers)

#define gl_BindRenderbuffer(target, renderbuffer) gl_call(glBindRenderbuffer, target, renderbuffer)

#define gl_RenderbufferStorage(target, internalformat, width, height) gl_call(glRenderbufferStorage, target, internalformat, width, height)

#define gl_FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer) gl_call(glFramebufferRenderbuffer, target, attachment, renderbuffertarget, renderbuffer)

#define gl_GenBuffers(n, buffers) gl_call(glGenBuffers, n, buffers)

#define gl_GenVertexArrays(n, arrays) gl_call(glGenVertexArrays, n, arrays)

#define gl_CheckFramebufferStatus(target) gl_call(glCheckFramebufferStatus, target)

#define gl_DeleteFramebuffers(n,  framebuffers) gl_call(glDeleteFramebuffers, n, framebuffers)

#define gl_DeleteTextures(n, textures) gl_call(glDeleteTextures, n, textures)

#define gl_DeleteRenderbuffers(n, renderbuffers) gl_call(glDeleteRenderbuffers, n, renderbuffers)

#define gl_GetFramebufferAttachmentParameteriv(target, attachment, pname, params) gl_call(glGetFramebufferAttachmentParameteriv, target, attachment, pname, params)

#define gl_CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height) gl_call(glCopyTexSubImage2D, target, level, xoffset, yoffset, x, y, width, height)

#define gl_Enable(cap) gl_call(glEnable, cap)

#define gl_Disable(cap) gl_call(glDisable, cap)

#define gl_BindVertexArray(array) gl_call(glBindVertexArray, array)

#define gl_BindBuffer(target, buffer) gl_call(glBindBuffer, target, buffer)

#define gl_BufferData(target, size, data, usage) gl_call(glBufferData, target, size, data, usage)

#define gl_DrawElements(mode, count, type, indices) gl_call(glDrawElements, mode, count, type, indices)

#define gl_DrawElementsInstanced(mode, count, type, indices, instancecount) gl_call(glDrawElementsInstanced, mode, count, type, indices, instancecount)

#define gl_DrawArraysInstanced(mode, first, count, instancecount) gl_call(glDrawArraysInstanced, mode, first, count, instancecount)

#define gl_DrawArrays(mode, first, count) gl_call(glDrawArrays, mode, first, count)

#define gl_UseProgram(program) gl_call(glUseProgram, program)

#define gl_GetUniformLocation(program, name) gl_call(glGetUniformLocation, program, name)

#define gl_Uniform1i(location, v0) gl_call(glUniform1i, location, v0)

#define gl_Uniform1ui(location, v0) gl_call(glUniform1ui, location, v0)

#define gl_Uniform1f(location, v0) gl_call(glUniform1f, location, v0)

#define gl_Uniform1iv(location, count, value) gl_call(glUniform1iv, location, count, value)

#define gl_Uniform1uiv(location, count, value) gl_call(glUniform1uiv, location, count, value);

#define gl_Uniform1fv(location, count, value) gl_call(glUniform1fv, location, count, value)

#define gl_UniformMatrix3fv(location, count, transpose, value) gl_call(glUniformMatrix3fv, location, count, transpose, value)

#define gl_UniformMatrix4fv(location, count, transpose, value) gl_call(glUniformMatrix4fv, location, count, transpose, value)

#define gl_GetShaderiv(shader, pname, params) gl_call(glGetShaderiv, shader, pname, params)

#define gl_GetShaderInfoLog(shader, maxLength, length, infoLog) gl_call(glGetShaderInfoLog, shader, maxLength, length, infoLog)

#define gl_GetProgramiv(program, pname, params) gl_call(glGetProgramiv, program, pname, params)

#define gl_GetProgramInfoLog(program, maxLength, length, infoLog) gl_call(glGetProgramInfoLog, program, maxLength, length, infoLog)

#define gl_CreateShader(shaderType) gl_call(glCreateShader, shaderType)

#define gl_ShaderSource(shader, count, string, length) gl_call(glShaderSource, shader, count, string, length)

#define gl_CompileShader(shader) gl_call(glCompileShader, shader)

#define gl_DeleteShader(shader) gl_call(glDeleteShader, shader)

#define gl_AttachShader(program, shader) gl_call(glAttachShader, program, shader)

#define gl_LinkProgram(program) gl_call(glLinkProgram, program)

#define gl_DeleteProgram(program) gl_call(glDeleteProgram, program)

#define gl_ActiveTexture(texture) gl_call(glActiveTexture, texture)

#define gl_GenerateMipmap(target) gl_call(glGenerateMipmap, target)

#define gl_VertexAttribPointer(index, size, type, normalized, stride, pointer) gl_call(glVertexAttribPointer, index, size, type, normalized, stride, pointer)

#define gl_EnableVertexAttribArray(index) gl_call(glEnableVertexAttribArray, index)

#define gl_VertexAttribDivisor(index, divisor) gl_call(glVertexAttribDivisor, index, divisor)

#define gl_Viewport(x, y, width, height) gl_call(glViewport, x, y, width, height)

namespace cxxgctx
{
namespace gl
{
extern void _glfw_error_print(int err_code, const char* description);
extern void _glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);

extern bool load_gl();
extern bool load_glfw(int major = 3, int minor = 3, GLFWerrorfun callback = cxxgctx::gl::_glfw_error_print);
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
	gl_GetIntegerv(param, (GLint* )&result);
	return result;
}

template<typename Derived>
class gl_object: public cxxcomm::bindable_object<GLuint, Derived>
{
public:
	gl_object() = default;

	gl_object(GLuint existed_id) :
			cxxcomm::bindable_object<GLuint, Derived>(existed_id)
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

#endif//_CXXGCTX_GL_GLBASIC
