#ifndef ANDROMEDA_GRAPHICS_GLUTILS
#define ANDROMEDA_GRAPHICS_GLUTILS

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
class gl_component
{
private:
	GLuint prev_bind_id = 0; //绑定本组件之前的上一个组件ID

protected:
	GLuint gl_id = 0; //本组件的ID

public:
	gl_component() = default;

	gl_component(GLuint existed_id) :
			gl_id(existed_id)
	{

	}

	~gl_component() = default;

	inline void invalidate()
	{
		Derived::invalidate(gl_id);
	}

	inline operator GLuint()
	{
		return gl_id;
	}

	inline GLuint id()
	{
		return gl_id;
	}

	template<typename ...Args>
	inline void use(Args ... args)
	{
		Derived::bind(gl_id, args...);
	}

	/**
	 * 绑定本对象并记录上次绑定的对象
	 */
	template<typename ...Args>
	__attribute__((always_inline)) inline void begin_use(Args ... args)
	{
		prev_bind_id = Derived::current_bind();
		Derived::bind(gl_id, args...);
	}

	/**
	 * 本对象解绑并绑定回上次绑定的对象
	 */
	template<typename ...Args>
	__attribute__((always_inline)) inline void end_use(Args ... args)
	{
		Derived::bind(prev_bind_id, args...);
		prev_bind_id = 0;
	}
};

}
}

#endif//ANDROMEDA_GRAPHICS_GLUTILS
