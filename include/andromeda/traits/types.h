#ifndef ANDROMEDA_TRAITS_TYPES
#define ANDROMEDA_TRAITS_TYPES

#include "macros.h"

/**
 * @brief 左值强制类型转换
 */
template<typename T1, typename T2>
__attribute__((always_inline)) inline constexpr T1& cast(T2& lv)
{
	return *(T1*)(&lv);
}

template<typename T1, typename T2>
__attribute__((always_inline)) inline constexpr T1& cast(T2&& rv)
{
	return *(T1*)(&(const T2&)rv);
}

//判断两个类型是否相同
template<typename T1, typename T2>
struct type_equal
{
	static const bool result = false;
};

template<typename T>
struct type_equal<T, T>
{
	static const bool result = true;
};

/**
 * @brief 声明变量，相关函数只有声明没有定义，只能在decltype()表达式中使用
 */
template<typename T>
struct __decl_impl
{
	typedef T result_type;

	static constexpr result_type val() noexcept;

	static constexpr result_type& ref() noexcept;

	static constexpr result_type* ptr() noexcept;

	static constexpr result_type&& x() noexcept;
};

template<typename T>
struct decl: __decl_impl<T>
{
	using __decl_impl<T>::result_type;
};

template<typename RetType, typename ... ArgTypes>
struct decl<RetType(ArgTypes...)> : __decl_impl<RetType (*)(ArgTypes...)>
{
	using __decl_impl<RetType (*)(ArgTypes...)>::result_type;
};

template<typename T>
constexpr T callexpr(T) noexcept;

template<typename Class, typename T>
constexpr T callexpr(T Class::*) noexcept;

/**
 * @brief 得到函数返回值类型，由于只有声明没有定义，该函数仅能在decltype()内用作类型推导
 */
template<typename RetType, typename ... ArgTypes>
constexpr RetType callexpr(RetType (*)(ArgTypes...)) noexcept;

template<typename Class, typename RetType, typename ... ArgTypes>
constexpr RetType callexpr(RetType (Class::*)(ArgTypes...)) noexcept;

template<typename T>
struct return_type
{
	typedef decltype(callexpr(decl<T>::val())) result_type; //结果类型
};

/**
 * @brief 判断Cond是否为true，如果是则result_type类型为TrueType，否则为FalseType
 */
template<bool Cond>
struct _if
{
	static_assert(false, "not all possible implementions found for _if<bool Cond>");
};

template<>
struct _if<true>
{
	static const bool result = true;

	/**
	 * @brief 可用于选择性实例化类模板
	 */
	template<typename TrueType, typename FalseType = TrueType>
	struct resolve_type
	{
		typedef TrueType result_type;
	};

	template<typename Type>
	struct enable_type
	{
		typedef Type result_type;
	};

	template<typename Type>
	struct disable_type
	{
	};

	template<typename TrueReturn, typename FalseReturn>
	__attribute__((always_inline)) inline static TrueReturn _return(TrueReturn true_val, FalseReturn false_val) noexcept
	{
		return true_val;
	}

	/**
	 * @brief 条件成立时可以调用，否则忽略
	 */
	template<typename Callable, typename ...ArgTypes>
	__attribute__((always_inline)) inline static auto call(Callable callable, ArgTypes ...args) -> decltype(callable(args...))
	{
		return callable(args...);
	}
};

template<>
struct _if<false>
{
	static const bool result = false;

	template<typename TrueType, typename FalseType = TrueType>
	struct resolve_type
	{
		typedef FalseType result_type;
	};

	template<typename Type>
	struct enable_type
	{
	};

	template<typename Type>
	struct disable_type
	{
		typedef Type result_type;
	};

	template<typename TrueReturn, typename FalseReturn>
	__attribute__((always_inline)) inline static FalseReturn _return(TrueReturn true_val, FalseReturn false_val) noexcept
	{
		return false_val;
	}

	template<typename Callable, typename ...ArgTypes>
	__attribute__((always_inline)) inline static auto call(Callable callable, ArgTypes ...args) -> decltype(callable(args...))
	{
	}
};

enum type_classification : int
{
	VALUE, //普通值
	FUNCTION, //普通函数
	MEMB_FIELD, //成员字段
	MEMB_FUNCTION, //成员函数
};

template<int Idx, typename First, typename ... Params>
struct __parameters_at_impl
{
	typedef typename _if<Idx <= 0 || sizeof...(Params) == 0>::resolve_type<First, __parameters_at_impl <Idx - 1, Params...> >::result_type result_type;
};

/**
 * @brief 获取指定索引的参数类型，索引从0开始，如果索引超出有效索引最小/最大范围，则取第一个/最后一个值
 */
template<int Idx, typename ... Params>
struct parameters_at: __parameters_at_impl<Idx, Params...>
{
	using __parameters_at_impl<Idx, Params...>::result_type;
};

/**
 * @brief 参数包，用于保存模板参数而不展开，对于非类型参数，使用constexpr_value<>封装
 */
template<typename ...Params>
struct parameters_pack
{
	/**
	 * @brief 将参数包用作函数参数并获取函数指针的类型
	 */
	template<typename Ret>
	struct as_func_args
	{
		typedef Ret (*func_type)(Params ...);
		typedef Ret return_type;
	};

	/**
	 * @brief 将参数包用作成员函数参数并获取成员函数指针的类型
	 */
	template<typename Class, typename Ret>
	struct as_memb_func_args
	{
		typedef Ret (Class::*func_type)(Params ...);
		typedef Ret return_type;
	};

	template<int Idx>
	struct at
	{
		typedef typename parameters_at<Idx, Params...>::result_type result_type;
	};
};

template<int Idx, typename ParamsPack>
struct parameters_pack_at
{
	static_assert(false, "parameters_pack_at's template parameter 'ParamsPack' should be type parameters_pack<...>");
};

/**
 * @brief 从parameters_pack<...>参数包中提取某个索引的类型，Params只接收parameters_pack<...>类型，其他类型将静态断言失败
 */
template<int Idx, typename ... Params>
struct parameters_pack_at<Idx, parameters_pack<Params...> >
{
	typedef typename parameters_at<Idx, Params...>::result_type result_type;
};

/**
 * @brief 值的种类判断
 */
template<typename T>
inline constexpr type_classification type_classification_of(T value)
{
	return type_classification::VALUE;
}

template<typename Ret, typename ... Args>
inline constexpr type_classification type_classification_of(Ret (*value)(Args...))
{
	return type_classification::FUNCTION;
}

template<typename Class, typename T>
inline constexpr type_classification type_classification_of(T Class::*value)
{
	return type_classification::MEMB_FIELD;
}

template<typename Class, typename Ret, typename ... Args>
inline constexpr type_classification type_classification_of(Ret (Class::*value)(Args...))
{
	return type_classification::MEMB_FUNCTION;
}

/**
 * @brief 将常量值转换成类型
 */
template<typename T, T Value>
struct constexpr_value
{
	static const type_classification classification = type_classification_of(Value);

	typedef T result_type;
	static constexpr T result = Value;
};

#define constexpr_val(value) constexpr_value<decltype(value), value>

/**
 * @brief 变量或函数所属的类
 */
template<typename T>
struct declaring_class
{
	typedef void result_type;
};

template<typename Class, typename T>
struct declaring_class<T Class::*>
{
	typedef Class result_type;
};

template<typename T>
struct __degenerated_type_impl
{
	typedef T result_type;
};

template<typename Class, typename T>
struct __degenerated_type_impl<T Class::*>
{
	typedef T result_type;
};

/**
 * @brief 成员指针退化，将成员字段及函数转换为非成员类型，成员函数则失去this参数
 */
template<typename T>
struct degenerated_type: public __degenerated_type_impl<T>
{
	typedef typename __degenerated_type_impl<T>::result_type result_type;
};

/**
 * @brief 包装返回值、参数、所属类以得到函数指针的类型，Class=void则视作普通函数类型，否则视作成员函数类型，函数类型转函数指针使用std::decay
 */
template<typename Class, typename RetType, typename ... ArgTypes>
struct func_type
{
	typedef typename _if<type_equal<Class, void>::result>::resolve_type<RetType (*)(ArgTypes...), RetType (Class::*)(ArgTypes...)>::result_type result_type;
};

template<typename T>
struct is_ptr
{
	static const bool result = false;
};

template<typename T>
struct is_ptr<T*>
{
	static const bool result = true;
};

template<typename T>
struct is_ref
{
	static const bool result = false;
};

template<typename T>
struct is_ref<T&>
{
	static const bool result = true;
};

template<typename T>
struct is_x
{
	static const bool result = false;
};

template<typename T>
struct is_x<T&&>
{
	static const bool result = true;
};

/**
 * 可以得到指针、引用等的原本类型，即传入T*或T**甚至更高次数的指针可以得到T
 */
template<typename T>
struct type_name
{
	typedef T result_type;
};

template<typename T>
struct type_name<T*>
{
	typedef T result_type;
};

template<typename T>
struct type_name<T&>
{
	typedef T result_type;
};

template<typename T>
struct type_name<T&&>
{
	typedef T result_type;
};

//判断是否是基础类型，如果是则结果为true，如果不是（是类）则返回false。可以传入指针、引用等，得到的依旧是原本类型
template<typename T>
struct is_primitive
{
	static const bool result = false;
};

#define	__define_primitive_type__(T)\
		template<>\
		struct is_primitive<T>\
		{\
			static const bool result = true;\
		};

#define	__define_primitive_types__(...) __repeat_each__(__define_primitive_type__, ##__VA_ARGS__)

__define_primitive_types__(
		char, unsigned char,
		short int, unsigned short int,
		int, unsigned int, long int, unsigned long int,
		long long int, unsigned long long int,
		float, double, long double, bool, void
		)

#undef __define_primitive_types__
#undef __define_primitive_type__

/**
 * 判断T是否是类，结果实际上为!(is_primitive_type<T>)
 */
template<typename T>
struct is_class
{
	static const bool result = !is_primitive<T>::result;
};

/**
 * @brief 构造类成员类型
 */
template<typename Class, typename T>
struct memb_ptr
{
	static const type_classification classification = type_classification::MEMB_FIELD;
	typedef T Class::*result_type;
};

template<typename Class, typename Ret, typename ...Args>
struct memb_ptr<Class, Ret(Args...)>
{
	static const type_classification classification = type_classification::MEMB_FUNCTION;
	typedef Ret (Class::*result_type)(Args...);

	typedef Ret return_type;
	typedef parameters_pack<Args...> args_type;
};

template<typename Class, typename Ret, typename ...Args>
struct memb_ptr<Class, Ret (*)(Args...)>
{
	static const type_classification classification = type_classification::MEMB_FUNCTION;
	typedef Ret (Class::*result_type)(Args...);

	typedef Ret return_type;
	typedef parameters_pack<Args...> args_type;
};

#endif//ANDROMEDA_TRAITS_TYPES