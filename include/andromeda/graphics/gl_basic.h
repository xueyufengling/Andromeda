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
extern void clear_gl_error();

/**
 * 返回错误列表的下一个错误
 */
extern GLenum next_gl_error();

/**
 * 查询一个GL状态值，例如查询当前绑定的ID
 */
__attribute__((always_inline)) inline GLuint gl_get_integer(GLuint param)
{
	GLuint result = 0;
	glGetIntegerv(param, (GLint*)&result);
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

}
}

#endif//ANDROMEDA_GRAPHICS_GLBASIC
