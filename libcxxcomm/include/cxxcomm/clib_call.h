#ifndef _CXXCOMM_CLIBCALL
#define _CXXCOMM_CLIBCALL

#include <cxxtricks/types.h>
#include <cxxtricks/callable.h>

#include <cxxcomm/log.h>

#define clibcall_name(lib_name) __cats__(2)(__clibcall_, lib_name)

/**
 * @brief 调用任意使用函数查询错误码的C库函数并返回执行的结果或错误码。
 * @param lib_name 库名称
 * @param errcode_type 错误码类型
 * @param errcode_val_macro 获取当前操作的错误码或成功码，该宏接收参数为目标函数返回值
 * @param errcode_noerr 无错误的码值
 * @return	调用void返回类型的库函数，本函数实际返回操作的成功或错误码；
 * 		  	调用具有返回值的库函数，则不会保存错误码只会打印到日志，并返回执行结果
 */
#define decl_clibcall(lib_name, errcode_type, errcode_val_macro, errcode_noerr)\
template<typename _Callable, typename ..._ArgTypes>\
__attribute__((always_inline)) inline auto clibcall_name(lib_name)(cxxcomm::log_level msg_level, const char* source_file_name, const char* source_line, const char* source_func_name, const char* source_func_paren, const char* callable_name, _Callable callable, _ArgTypes ... args) ->\
	typename _if<type_equal<decltype(callable(args...)), void>::value>::resolve<errcode_type, decltype(callable(args...))>::type\
{\
	callable_retv<_Callable> call(callable);\
	decltype(call(args...)) ret;\
	errcode_type err_code = errcode_noerr;\
	ret = call(args...);\
	if((err_code = (errcode_val_macro(ret))) != errcode_noerr)\
	{\
		LogSource(msg_level, source_file_name, source_line, source_func_name, source_func_paren, __str__(lib_name) " call ", callable_name, "() failed. error code: ", err_code);\
	}\
	return _if<type_equal<decltype(callable(args...)), void>::value>::_return(err_code, ret);\
}

/**
 * @brief 调用库函数并打印错误码。打印的源码定位由参数指定
 * 		  使用时传入clibcall_source(lib_name, __log_source__, callable, ...)
 */
#define clibcall(lib_name, level, log_source, callable, ...) clibcall_name(lib_name)(level, log_source, __str__(callable), callable,  ##__VA_ARGS__)

#endif //_CXXCOMM_CLIBCALL
