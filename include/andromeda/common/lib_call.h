#ifndef ANDROMEDA_COMMON_LIBCALL
#define ANDROMEDA_COMMON_LIBCALL

#include "../traits/types.h"
#include "../traits/call.h"
#include "log.h"

/**
 * @brief 调用任意库函数并返回操作的成功或错误码
 */
#define LibCall(func_name, errcode_type, errcode_func)\
template<typename Ret, typename Callable, typename ...Args>\
__attribute__((always_inline)) inline errcode_type func_name(Callable callable, Ret* ret, Args ... args)\
{\
	if(ret)\
		*ret = callable(args...);\
	else\
		callable(args...);\
	return errcode_func();\
}

/**
 * @brief 调用任意库函数并返回执行的结果或错误码。
 * @return	调用void返回类型的库函数，本函数实际返回操作的成功或错误码；
 * 		  	调用具有返回值的库函数，则不会保存错误码只会打印到日志，并返回执行结果
 */
#define LibCallRet(func_name, errcode_type, errcode_func, errcode_noerr)\
template<typename Callable, typename ...Args>\
__attribute__((always_inline)) inline auto func_name(Callable callable, Args ... args) -> \
		typename _if<type_equal<decltype(callable(args...)), void>::result, errcode_type, decltype(callable(args...))>::result_type\
{\
	call_ret<Callable> call(callable);\
	decltype(call(args...)) ret;\
	errcode_type err_code = errcode_noerr;\
	ret = call(args...);\
	if((err_code = errcode_func()) != errcode_noerr)\
	{\
		LogError(#func_name " call lib function failed. error code: ", err_code);\
	}\
	return _if<type_equal<decltype(callable(args...)), void>::result>::_return(err_code, ret);\
}

#define LibCallAll(func_name, errcode_type, errcode_func, errcode_noerr)\
		LibCall(func_name, errcode_type, errcode_func)\
		LibCallRet(func_name##Ret, errcode_type, errcode_func, errcode_noerr)

#endif //ANDROMEDA_COMMON_LIBCALL
