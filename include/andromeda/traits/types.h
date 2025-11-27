#ifndef ANDROMEDA_TRAITS_TYPES
#define ANDROMEDA_TRAITS_TYPES

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

/**
 * @brief 判断Cond是否为true，如果是则result_type类型为TrueType，否则为FalseType
 */
template<bool Cond, typename TrueType = void, typename FalseType = void>
struct _if
{
};

template<typename TrueType, typename FalseType>
struct _if<true, TrueType, FalseType>
{
	typedef TrueType result_type;

	static const bool result = true;

	/**
	 * @brief 防止模板实例化后声明void类型的变量
	 */
	template<typename True_return, typename False_return>
	__attribute__((always_inline)) inline static True_return _return(True_return true_val, False_return false_val) noexcept
	{
		return true_val;
	}

	template<typename True_call, typename False_call, typename ...ArgTypes>
	__attribute__((always_inline)) inline static auto call(True_call true_func, False_call false_func, ArgTypes ...args) -> decltype(true_func(args...))
	{
		return true_func(args...);
	}
};

template<typename TrueType, typename FalseType>
struct _if<false, TrueType, FalseType>
{
	typedef FalseType result_type;

	static const bool result = false;

	template<typename True_return, typename False_return>
	__attribute__((always_inline)) inline static False_return _return(True_return true_val, False_return false_val) noexcept
	{
		return false_val;
	}

	template<typename True_call, typename False_call, typename ...ArgTypes>
	__attribute__((always_inline)) inline static auto call(True_call true_func, False_call false_func, ArgTypes ...args) -> decltype(false_func(args...))
	{
		return false_func(args...);
	}
};

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
 * @brief 声明变量
 */
template<typename T>
struct decl
{
	typedef T result_type;

	static_assert(!type_equal<T,void>::result, "decl<typename T> cannot declare void val");

	static result_type val;
	static result_type* ptr;

	/**
	 * @brief 将亡值
	 */
	__attribute__((always_inline)) inline static constexpr result_type&& x() noexcept
	{
		return 0;
	}
};

/**
 * @brief 函数类型的变量
 */
template<typename Ret, typename ...Args>
struct decl<Ret(Args...)>
{
	typedef Ret (*result_type)(Args...);

	static result_type val;
	static result_type ptr;

	__attribute__((always_inline)) inline static constexpr result_type x() noexcept
	{
		return 0;
	}
};

/**
 * @brief 得到函数返回值类型，由于只有声明没有定义，该函数仅能在decltype()内用作类型推导
 */
template<typename RetType, typename ... ArgTypes>
RetType __return_type(RetType (*)(ArgTypes...));

template<typename Class, typename RetType, typename ... ArgTypes>
RetType __return_type(RetType (Class::*)(ArgTypes...));

template<typename Func>
struct return_type
{
	typedef decltype(__return_type(decl<Func>::val)) result_type; //结果类型
};

template<typename Func>
struct __func_class_impl
{
	typedef void result_type;
};

template<typename Class, typename Func>
struct __func_class_impl<Func Class::*>
{
	typedef Class result_type;
};

/**
 * @brief 函数所属的类
 */
template<typename Func>
struct func_class: public __func_class_impl<Func> //C++11类模板参数没有自动推导，因此需要通过继承自动推导
{
	typedef typename __func_class_impl<Func>::result_type result_type;
};

template<typename Func>
struct __degenerate_type_impl
{
	typedef Func result_type;
};

template<typename Class, typename Func>
struct __degenerate_type_impl<Func Class::*>
{
	typedef Func result_type;
};

/**
 * @brief 函数退化，将所有函数转换为非成员函数，成员函数则失去this参数
 */
template<typename Func>
struct degenerate_type: public __degenerate_type_impl<Func>
{
	typedef typename __degenerate_type_impl<Func>::result_type result_type;
};

template<typename Func>
struct is_member_func
{
	static const bool result = !type_equal<typename func_class<Func>::result_type, void>::result;
};

/**
 * @brief 包装返回值、参数、所属类以得到函数指针的类型，Class=void则视作普通函数类型，否则视作成员函数类型，函数类型转函数指针使用std::decay
 */
template<typename Class, typename RetType, typename ... ArgTypes>
struct func_type
{
	typedef typename _if<type_equal<Class, void>::result, RetType (*)(ArgTypes...), RetType (Class::*)(ArgTypes...)>::result_type result_type;
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

#define	DEFINE_PRIMITIVE_TYPE(T)\
		template<>\
		struct is_primitive<T>\
		{\
			static const bool result = true;\
		};
//8位
DEFINE_PRIMITIVE_TYPE(char)
DEFINE_PRIMITIVE_TYPE(unsigned char)
//16位
DEFINE_PRIMITIVE_TYPE(short int)
DEFINE_PRIMITIVE_TYPE(unsigned short int)
//32位
DEFINE_PRIMITIVE_TYPE(int)
DEFINE_PRIMITIVE_TYPE(unsigned int)
//32位
DEFINE_PRIMITIVE_TYPE(long int)
DEFINE_PRIMITIVE_TYPE(unsigned long int)
//64位
DEFINE_PRIMITIVE_TYPE(long long int)
DEFINE_PRIMITIVE_TYPE(unsigned long long int)
//32位
DEFINE_PRIMITIVE_TYPE(float)
//64位
DEFINE_PRIMITIVE_TYPE(double)
//64位
DEFINE_PRIMITIVE_TYPE(long double)
//32位
DEFINE_PRIMITIVE_TYPE(bool)
//void类型
DEFINE_PRIMITIVE_TYPE(void)

DEFINE_PRIMITIVE_TYPE(void*)

#undef DEFINE_PRIMITIVE_TYPE

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
	typedef T Class::*result_type;
};

template<typename Class, typename Ret, typename ...Args>
struct memb_ptr<Class, Ret(Args...)>
{
	typedef Ret (Class::*result_type)(Args...);
};

#endif//ANDROMEDA_TRAITS_TYPES