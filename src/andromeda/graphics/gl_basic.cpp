#include <andromeda/common/log.h>
#include <andromeda/graphics/gl_basic.h>

#include <string.h>

static bool gl_loaded = false;

void andromeda::graphics::_glfw_error_print(int err_code, const char* description)
{
	LogError("GLFW Error Code:", err_code, "\nDescription:", description)
}

void andromeda::graphics::_glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	gl_Viewport(0, 0, width, height);
}

bool andromeda::graphics::load_gl()
{
	if(!gl_loaded)
	{
		if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			gl_loaded = true;
		else
			LogFatal("Initialize GLAD failed");
	}
	return gl_loaded;
}

bool andromeda::graphics::load_glfw(int major, int minor, GLFWerrorfun callback)
{
	if(!glfwInit())
	{
		LogFatal("Initialize GLFW failed");
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwSetErrorCallback(callback);
	return true;
}

void andromeda::graphics::term_glfw()
{
	glfwTerminate();
}

void andromeda::graphics::gl_clear_error()
{
	while(glGetError() != GL_NO_ERROR)
		;
}

GLenum andromeda::graphics::gl_next_error()
{
	while(GLenum error = glGetError())
		return error;
	return GL_NO_ERROR;
}

size_t andromeda::graphics::sizeof_gl_type(GLint type)
{
	switch(type)
	{
	case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		return sizeof(char);
	case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		return sizeof(short int);
	case GL_INT:
		case GL_UNSIGNED_INT:
		return sizeof(int);
	case GL_FLOAT:
		return sizeof(float);
	case GL_DOUBLE:
		return sizeof(double);
#ifdef GL_2_BYTES
	case GL_2_BYTES:
		return sizeof(char)*2;
#endif
#ifdef GL_3_BYTES
	case GL_3_BYTES:
		return sizeof(char)*3;
#endif
#ifdef GL_4_BYTES
	case GL_4_BYTES:
		return sizeof(char)*4;
#endif
	}
	return 0; //返回0表示查询失败，传入的type不是OpenGL的数据类型
}

GLuint andromeda::graphics::gl_type(const char* str)
{
	bool at_least_2_chars = strlen(str) > 1;
	switch(*str)
	{
	case 'b':
		return GL_BYTE;
	case 's':
		return GL_SHORT;
	case 'i':
		return GL_INT;
	case 'f':
		return GL_FLOAT;
	case 'd':
		return GL_DOUBLE;
	case 'u':
		if(at_least_2_chars)
			switch(*(str + 1))
			{
			case 'b':
				return GL_UNSIGNED_BYTE;
			case 's':
				return GL_UNSIGNED_SHORT;
			case 'i':
				return GL_UNSIGNED_INT;
			}
		break;
#ifdef GL_2_BYTES
		case '2':
		if(at_least_2_chars)
		if(*(str+1)=='b')
		return GL_2_BYTES;
		break;
#endif
#ifdef GL_3_BYTES
		case '3':
		if(at_least_2_chars)
		if(*(str+1)=='b')
		return GL_3_BYTES;
		break;
#endif
#ifdef GL_4_BYTES
		case '4':
		if(at_least_2_chars)
		if(*(str+1)=='b')
		return GL_4_BYTES;
		break;
#endif
	}
	return GL_NONE;
}
