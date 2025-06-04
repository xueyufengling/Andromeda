#ifndef ANDROMEDA_UTIL_GLUTILS
#define ANDROMEDA_UTIL_GLUTILS

#include <opengl/glad/glad.h>
#include <cstddef>

namespace andromeda {
	namespace util {
		extern size_t getGLTypeSize(GLint type); //获取OpenGL的数据类型大小
		/*通过字符串缩写获取OpenGL的类型，从字符串开头进行匹配，一旦成功则不论后面字符串的内容均直接返回类型
		 * b，ub，2b，3b，4b，s，us，i，ui，f，d
		 */
		extern GLuint getGLType(const char *str);
		extern void clearOpenGLError();
		extern void checkOpenGLError();
	}
}

#endif//ANDROMEDA_UTIL_GLUTILS
