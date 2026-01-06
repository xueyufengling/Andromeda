#ifndef _CXXTRICKS_CALLABLE
#define _CXXTRICKS_CALLABLE

#include <cxxtricks/types.h>

/**
 * @brief 将任意函数包装为带返回值的函数，防止在模板中出现void返回值作为右值。如果函数为void返回类型，则包装为返回int，且其返回值为0
 */
template<typename _FuncType, typename _RetType>
struct __callable_retv_impl
{
	typedef _RetType result_type;

	_FuncType _callable;

	__attribute__((always_inline)) inline __callable_retv_impl(_FuncType callable) :
			_callable(callable)
	{
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline result_type operator()(_ArgTypes ... args)
	{
		return _callable(args...);
	}
};

template<typename _FuncType>
struct __callable_retv_impl<_FuncType, void>
{
	typedef int result_type;

	_FuncType _callable;

	__attribute__((always_inline)) inline __callable_retv_impl(_FuncType callable) :
			_callable(callable)
	{
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline result_type operator()(_ArgTypes ... args)
	{
		_callable(args...);
		return 0;
	}
};

template<typename _FuncType>
struct callable_retv: public __callable_retv_impl<_FuncType, typename eval_type<_FuncType>::result_type>
{
	using __callable_retv_impl<_FuncType, typename eval_type<_FuncType>::result_type>::result_type;

	__attribute__((always_inline)) inline callable_retv(_FuncType callable) :
			__callable_retv_impl<_FuncType, typename eval_type<_FuncType>::result_type>(callable)
	{
	}
};

#endif //_CXXTRICKS_CALLABLE
