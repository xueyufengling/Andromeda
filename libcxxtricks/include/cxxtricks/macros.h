#ifndef _CXXTRICKS_MACROS
#define _CXXTRICKS_MACROS

#include <ppmp/ppmp.h>

/**
 * 声明友元类
 */
#define __friend_class__(...) __va_macro__(__friend_class__, __VA_ARGS__)
#define __friend_class__0(...)
#define __friend_class__1(cls) friend class cls;

#define __friend_classes__(...) __repeat_each__(__friend_class__, __VA_ARGS__)

#define __template_friend_class__(cls, ...)\
	template<__VA_ARGS__>\
	friend class cls;

#define __template_friend_classes__(template_params_macro, ...) __repeat_each_extras__(__template_friend_class__, template_params_macro, __VA_ARGS__)

/**
 * @brief 用于显式实例化模板，只能传入一个待实例化的模板类型。
 */
#define __explicit_instantiate__(...) template class __VA_ARGS__;

#endif //_CXXTRICKS_MACROS
