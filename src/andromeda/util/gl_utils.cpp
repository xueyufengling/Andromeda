#include <andromeda/util/gl_utils.h>

#include <string.h>
#include "../../../include/andromeda/util/log.h"

using namespace andromeda::util;

void andromeda::util::clearOpenGLError()
{
	while(glGetError()!=GL_NO_ERROR)
		;
}

void andromeda::util::checkOpenGLError()
{
	while(GLenum error= glGetError())
		LOG_DEBUG("OpenGL error code:",error)
}

size_t andromeda::util::getGLTypeSize(GLint type)
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

GLuint andromeda::util::getGLType(const char* str)
{
	bool at_least_2_chars=strlen(str)>1;
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
			switch(*(str+1))
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
