#ifndef ANDROMEDA_TRAITS_CALL
#define ANDROMEDA_TRAITS_CALL

#include <functional>
#include <utility>
#include "types.h"

//得到绑定了cls=this的Class成员函数std::function，如果Class=void则不绑定this，指针视作普通函数指针。用法 bind_this<>::bind(func,this)
template<typename Class, typename RetType, typename ... ArgsType>
struct bind_this
{
	static std::function<RetType(ArgsType...)> bind(typename func_type<Class, RetType, ArgsType...>::result_type func, Class* cls)
	{
		return [&](ArgsType&& ... args)
			{
				return (cls->*func)(std::forward<decltype(args)>(args)...);
			};
	}
};

//普通函数指针的实现
template<typename RetType, typename ... ArgsType>
struct bind_this<void, RetType, ArgsType...>
{
	std::function<RetType(ArgsType...)> bind(typename func_type<void, RetType, ArgsType...>::result_type func, void* cls = nullptr)
	{
		return [&](ArgsType&& ... args)
			{
				return (*func)(std::forward<decltype(args)>(args)...);
			};
	}

};

/**
 * @brief 将任意函数包装为带返回值的函数，防止在模板中出现void返回值作为右值。如果函数为void返回类型，则包装为返回int，且其返回值为0
 */
template<typename Func, typename Ret>
struct __call_ret_impl
{
	typedef Ret result_type;

	Func _callable;

	__attribute__((always_inline)) inline __call_ret_impl(Func callable) :
			_callable(callable)
	{
	}

	template<typename ...ArgTypes>
	__attribute__((always_inline)) inline result_type operator()(ArgTypes ... args)
	{
		return _callable(args...);
	}
};

template<typename Func>
struct __call_ret_impl<Func, void>
{
	typedef int result_type;

	Func _callable;

	__attribute__((always_inline)) inline __call_ret_impl(Func callable) :
			_callable(callable)
	{
	}

	template<typename ...ArgTypes>
	__attribute__((always_inline)) inline result_type operator()(ArgTypes ... args)
	{
		_callable(args...);
		return 0;
	}
};

template<typename Func>
struct call_ret: public __call_ret_impl<Func, typename return_type<Func>::result_type>
{
	using __call_ret_impl<Func, typename return_type<Func>::result_type>::result_type;

	__attribute__((always_inline)) inline call_ret(Func callable) :
			__call_ret_impl<Func, typename return_type<Func>::result_type>(callable)
	{
	}
};

#endif //ANDROMEDA_TRAITS_CALL
