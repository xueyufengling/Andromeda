#ifndef _CXXJVMI_JVM
#define _CXXJVMI_JVM

/**
 * 此头文件依赖于JNI
 */
#include <jni.h>

namespace cxxjvmi
{
class jvm
{
protected:
	JavaVM* jvm;
	JNIEnv* env;

public:
	jvm(JavaVM* jvm, JNIEnv* env);
	jvm(JavaVM* jvm, JNIEnv* env);
};
}

#endif //_CXXJVMI_JVM
