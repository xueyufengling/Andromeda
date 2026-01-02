#ifndef ANDROMEDA_TRAITS_TYPES
#define ANDROMEDA_TRAITS_TYPES

#include "macros.h"

/**
 * @brief 左值强制类型转换
 */
template<typename _T1, typename _T2>
__attribute__((always_inline)) inline constexpr _T1& cast(_T2& lv)
{
	return *(_T1*)(&lv);
}

template<typename _T1, typename _T2>
__attribute__((always_inline)) inline constexpr _T1& cast(_T2&& rv)
{
	return *(_T1*)(&(const _T2&)rv);
}

//判断两个类型是否相同
template<typename _T1, typename _T2>
struct type_equal
{
	static const bool result = false;
};

template<typename _T>
struct type_equal<_T, _T>
{
	static const bool result = true;
};

/**
 * @brief 声明变量，val()、ref()、x()函数只有声明没有定义，只能在decltype()表达式中使用
 * 		  placeholder()返回值可以用于作为参数传递协助类型推导，但不能访问
 */
template<typename _T>
struct __decl_impl
{
public:
	typedef _T result_type;

	static constexpr result_type val() noexcept;

	static constexpr result_type&& x() noexcept;

	inline static constexpr result_type* ptr() noexcept
	{
		return nullptr;
	}

	/**
	 * @brief 返回一个占位符值，可以赋值、传值，但不能访问
	 * 		  此函数不是constexpr，编译时如果对nullptr解引用会抛出编译错误。但此函数仍可在编译时在给static const变量赋值表达式中使用
	 */
	static result_type& ref() noexcept
	{
		static result_type* _nullptr = ptr();
		return *_nullptr;
	}
};

template<typename _T>
struct decl: __decl_impl<_T>
{
	using __decl_impl<_T>::result_type;
};

template<typename _RetType, typename ... _ArgTypes>
struct decl<_RetType(_ArgTypes...)> : __decl_impl<_RetType (*)(_ArgTypes...)>
{
	using __decl_impl<_RetType (*)(_ArgTypes...)>::result_type;
};

template<typename _T>
constexpr _T callexpr(_T) noexcept;

template<typename _Class, typename _T>
constexpr _T callexpr(_T _Class::*) noexcept;

/**
 * @brief 得到函数返回值类型，由于只有声明没有定义，该函数仅能在decltype()内用作类型推导
 */
template<typename _RetType, typename ... _ArgTypes>
constexpr _RetType callexpr(_RetType (*)(_ArgTypes...)) noexcept;

template<typename _Class, typename _RetType, typename ... _ArgTypes>
constexpr _RetType callexpr(_RetType (_Class::*)(_ArgTypes...)) noexcept;

template<typename _T>
struct return_type
{
	typedef decltype(callexpr(decl<_T>::val())) result_type; //结果类型
};

/**
 * @brief 判断_Cond是否为true，如果是则result_type类型为_TrueType，否则为_FalseType
 */
template<bool _Cond>
struct _if
{
	static_assert(false, "not all possible implementions found for _if<bool _Cond>");
};

template<>
struct _if<true>
{
	static const bool result = true;

	/**
	 * @brief 可用于选择性实例化类模板
	 */
	template<typename _TrueType, typename _FalseType = _TrueType>
	struct resolve_type
	{
		typedef _TrueType result_type;
	};

	template<typename _T>
	struct enable_type
	{
		typedef _T result_type;
	};

	template<typename _T>
	struct disable_type
	{
	};

	template<typename _TrueReturn, typename _FalseReturn>
	__attribute__((always_inline)) inline static _TrueReturn _return(_TrueReturn true_val, _FalseReturn false_val) noexcept
	{
		return true_val;
	}

	/**
	 * @brief 条件成立时可以调用，否则忽略
	 */
	template<typename _Callable, typename ..._ArgTypes>
	__attribute__((always_inline)) inline static auto call(_Callable callable, _ArgTypes ...args) -> decltype(callable(args...))
	{
		return callable(args...);
	}
};

template<>
struct _if<false>
{
	static const bool result = false;

	template<typename _TrueType, typename _FalseType = _TrueType>
	struct resolve_type
	{
		typedef _FalseType result_type;
	};

	template<typename _T>
	struct enable_type
	{
	};

	template<typename _T>
	struct disable_type
	{
		typedef _T result_type;
	};

	template<typename _TrueReturn, typename _FalseReturn>
	__attribute__((always_inline)) inline static _FalseReturn _return(_TrueReturn true_val, _FalseReturn false_val) noexcept
	{
		return false_val;
	}

	template<typename _Callable, typename ..._ArgTypes>
	__attribute__((always_inline)) inline static auto call(_Callable callable, _ArgTypes ...args) -> decltype(callable(args...))
	{
	}
};

struct bool_type
{
	struct _true;
	struct _false;

	template<typename _T>
	static constexpr bool bool_of(void)
	{
		return type_equal<_T, _true>::result;
	}

	template<bool _Value>
	struct of
	{
		typedef typename _if<_Value>::resolve_type<_true, _false>::result_type result_type;
	};
};

/**
 * 得到表达式的bool_type返回类型对应的值，不会实际计算表达式，能避免一些编译错误
 */
#define bool_of_expr(expr) bool_type::bool_of<decltype(expr)>()

/**
 * @brief 用作匹配具有非模板重载的重载模板函数的占位符。
 * 		  这个占位符对应的非模板重载函数参数类型必须是_T1/_T2，重载模板函数参数类型必须是_T2/_T1，且这两个参数必须传入本类对象。
 * 		  非模板重载函数的参数类型为_T1，以匹配的重载模板函数参数类型为_T2为例：
 * 		  (1)如果_T2可以转换为_T1，则目标重载模板函数的_T1模板重载和_T2模板重载均为有效候选者，但
 * 		  _T1模板重载接收const __conversion_placeholder* this参数，最终为_T1；
 * 		  _T2模板重载接收__conversion_placeholder* this参数，之后_T2可以二次转换最终为_T1。
 * 		  在通常情况下，这两个模板函数实例需要按照后述规则1，根据调用对象是否有const修饰符确定优先级。
 * 		  如果调用对象为非const对象，则优先调用operator _T2&()；若为const对象，则优先调用operator _T1&() const
 * 		  (2)如果_T2不能转换为_T1，则目标重载模板函数的_T1模板重载和_T2模板重载均为有效候选者，但
 * 		  _T1模板重载接收const __conversion_placeholder* this参数，最终为_T1；
 * 		  _T2模板重载接收__conversion_placeholder* this参数，最终为_T2。
 * 		  两种重载都合法，因此需要考虑非模板重载进一步决议。
 * 		  _T2模板重载与_T2非重载模板函数都接收非const对象，优先级高于_T1模板重载，但由于优先匹配非模板函数，因此重载决议为非模板重载函数_T2.
 * 		  注：在同等优先级下，函数匹配优先非模板函数，仅当非模板函数不存在时才实例化重载模板函数。函数匹配的优先级为：
 * 		  1. 参数类型完全匹配，包括形参和实参的const修饰符也能匹配
 * 		  2. 形参接收const T，实参为T
 * 		  3. 推广转换，小类型无损自动转为大类型，例如形参接收long long int，实参为int
 * 		  4. 标准转换，参数类型原生的可隐式转换，例如整形转浮点型，派生类->基类
 * 		  5. 用户定义转换（UDC），即构造函数和operator T()
 * 		  6. 能匹配上述任意一条规则，且具有变长参数列表，即最后形参为...
 * 		  综上所述，当传入实参为非const对象时，若_T2可转换为_T1，则始终匹配_T2&模板重载实例化；否则始终匹配_T1&非模板重载
 */
template<typename _T1, typename _T2>
struct __conversion_placeholder
{
	/**
	 * const关键字用于标记operator _T1&()是const函数，即operator _T1&(const __conversion_placeholder* this)，this为隐藏参数，其作用为：
	 * 1. 当_T1 = _T2时，两个operator转换函数不会导致重定义
	 * 2. const对象调用优先级更高
	 */
	inline operator _T1&() const
	{
		return decl<_T1>::ref();
	}

	inline operator _T1*() const
	{
		return decl<_T1>::ptr();
	}

	/**
	 * operator _T2&(__conversion_placeholder* this)，this为隐藏参数
	 * 非const对象调用优先级更高
	 */
	inline operator _T2&()
	{
		return decl<_T2>::ref();
	}

	inline operator _T2*()
	{
		return decl<_T2>::ptr();
	}

	/**
	 * @brief 非const对象
	 */
	__conversion_placeholder() = default;

	/**
	 * @brief const对象
	 */
	static constexpr __conversion_placeholder<_T1, _T2> _const{};
};

/**
 * @brief 测试_Original -> _Target是否可转换。
 * 		  包含隐式转换、用户自定义转换，通过函数重载决议优先级区分是否可转换，不需要实际的转换计算，因此避免了private、protected继承的情况下因权限问题而抛出编译错误
 */
template<typename _Original, typename _Target>
struct is_convertible
{
private:
	/**
	 * 注：此处不是SFINAE（因此不能使用变长参数列表...），而是函数重载决议优先级，即从多个有效候选函数中决议出最优的那个使用。
	 * 为了保证非模板函数优先匹配，第二个参数必须与传入__check()的第二个参数类型完全一致
	 */
	static bool_type::_false __check(_Target, int) noexcept;

	template<typename _Any>
	static bool_type::_true __check(_Original, _Any) noexcept;

public:
	/**
	 * 非const对象，若可转换则__conversion_placeholder()返回类型为_Original，否则返回类型有两种候选：_Original、_Target，需要与非模板函数进一步匹配决议。
	 * bool_of_expr()内部没有实际进行计算，不会抛出编译错误
	 */
	static const bool result = bool_of_expr(__check(__conversion_placeholder<_Target, _Original>(), int()));
};

enum type_classification : int
{
	VALUE, //普通值
	FUNCTION, //普通函数
	MEMB_FIELD, //成员字段
	MEMB_FUNCTION, //成员函数
};

template<int _Index, typename _First, typename ... _Params>
struct __parameters_at_impl
{
	typedef typename _if<_Index <= 0 || sizeof...(_Params) == 0>::resolve_type<_First, __parameters_at_impl <_Index - 1, _Params...> >::result_type result_type;
};

/**
 * @brief 获取指定索引的参数类型，索引从0开始，如果索引超出有效索引最小/最大范围，则取第一个/最后一个值
 */
template<int _Index, typename ... _Params>
struct parameters_at: __parameters_at_impl<_Index, _Params...>
{
	using __parameters_at_impl<_Index, _Params...>::result_type;
};

/**
 * @brief 参数包，用于保存模板参数而不展开，对于非类型参数，使用constexpr_value<>封装
 */
template<typename ..._Params>
struct parameters_pack
{
	/**
	 * @brief 将参数包用作函数参数并获取函数指针的类型
	 */
	template<typename _RetType>
	struct as_func_args
	{
		typedef _RetType (*func_type)(_Params ...);
		typedef _RetType return_type;
	};

	/**
	 * @brief 将参数包用作成员函数参数并获取成员函数指针的类型
	 */
	template<typename _Class, typename _RetType>
	struct as_memb_func_args
	{
		typedef _RetType (_Class::*func_type)(_Params ...);
		typedef _RetType return_type;
	};

	template<int _Index>
	struct at
	{
		typedef typename parameters_at<_Index, _Params...>::result_type result_type;
	};
};

template<int _Index, typename _ParamsPack>
struct parameters_pack_at
{
	static_assert(false, "parameters_pack_at's template parameter '_ParamsPack' should be type parameters_pack<...>");
};

/**
 * @brief 从parameters_pack<...>参数包中提取某个索引的类型，_Params只接收parameters_pack<...>类型，其他类型将静态断言失败
 */
template<int _Index, typename ... _Params>
struct parameters_pack_at<_Index, parameters_pack<_Params...> >
{
	typedef typename parameters_at<_Index, _Params...>::result_type result_type;
};

/**
 * @brief 值的种类判断
 */
template<typename _T>
inline constexpr type_classification type_classification_of(_T value)
{
	return type_classification::VALUE;
}

template<typename _RetType, typename ... _ArgTypes>
inline constexpr type_classification type_classification_of(_RetType (*value)(_ArgTypes...))
{
	return type_classification::FUNCTION;
}

template<typename _Class, typename _T>
inline constexpr type_classification type_classification_of(_T _Class::*value)
{
	return type_classification::MEMB_FIELD;
}

template<typename _Class, typename _RetType, typename ... _ArgTypes>
inline constexpr type_classification type_classification_of(_RetType (_Class::*value)(_ArgTypes...))
{
	return type_classification::MEMB_FUNCTION;
}

/**
 * @brief 将常量值转换成类型
 */
template<typename _T, _T _Value>
struct constexpr_value
{
	static const type_classification classification = type_classification_of(_Value);

	typedef _T result_type;
	static constexpr result_type result = _Value;
};

#define constexpr_val(value) constexpr_value<decltype(value), value>

/**
 * @brief 变量或函数所属的类
 */
template<typename _T>
struct declaring_class
{
	typedef void result_type;
};

template<typename _Class, typename _T>
struct declaring_class<_T _Class::*>
{
	typedef _Class result_type;
};

template<typename _T>
struct __degenerated_type_impl
{
	typedef _T result_type;
};

template<typename _Class, typename _T>
struct __degenerated_type_impl<_T _Class::*>
{
	typedef _T result_type;
};

/**
 * @brief 成员指针退化，将成员字段及函数转换为非成员类型，成员函数则失去this参数
 */
template<typename _T>
struct degenerated_type: public __degenerated_type_impl<_T>
{
	typedef typename __degenerated_type_impl<_T>::result_type result_type;
};

/**
 * @brief 包装返回值、参数、所属类以得到函数指针的类型，_Class=void则视作普通函数类型，否则视作成员函数类型，函数类型转函数指针使用std::decay
 */
template<typename _Class, typename _RetType, typename ... _ArgTypes>
struct func_type
{
	typedef typename _if<type_equal<_Class, void>::result>::resolve_type<_RetType (*)(_ArgTypes...), _RetType (_Class::*)(_ArgTypes...)>::result_type result_type;
};

template<typename _T>
struct is_ptr
{
	static const bool result = false;
};

template<typename _T>
struct is_ptr<_T*>
{
	static const bool result = true;
};

template<typename _T>
struct is_ref
{
	static const bool result = false;
};

template<typename _T>
struct is_ref<_T&>
{
	static const bool result = true;
};

template<typename _T>
struct is_x
{
	static const bool result = false;
};

template<typename _T>
struct is_x<_T&&>
{
	static const bool result = true;
};

/**
 * 可以得到指针、引用等的原本类型，即传入T*或T**甚至更高次数的指针可以得到T
 */
template<typename _T>
struct type_name
{
	typedef _T result_type;
};

template<typename _T>
struct type_name<_T*>
{
	typedef _T result_type;
};

template<typename _T>
struct type_name<_T&>
{
	typedef _T result_type;
};

template<typename _T>
struct type_name<_T&&>
{
	typedef _T result_type;
};

//判断是否是基础类型，如果是则结果为true，如果不是（是类）则返回false。可以传入指针、引用等，得到的依旧是原本类型
template<typename _T>
struct is_primitive
{
	static const bool result = false;
};

#define	__define_primitive_type__(_T)\
		template<>\
		struct is_primitive<_T>\
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
 * 判断T是否是类，结果实际上为!(is_primitive_type<_T>)
 */
template<typename _T>
struct is_class
{
	static const bool result = !is_primitive<_T>::result;
};

/**
 * 检测是否存在继承关系
 */
template<typename _Derived, typename _Base>
struct is_derived
{
	static const bool result = is_class<_Derived>::result && is_class<_Base>::result && is_convertible<_Derived, _Base>::result;
};

/**
 * 检测是否可赋值
 */
template<typename _Src, typename _Target>
struct is_assignable
{
	static const bool result = type_equal<_Src, _Target>::result || is_convertible<_Src, _Target>::result;
};

/**
 * @brief 构造类成员类型
 */
template<typename _Class, typename _T>
struct memb_ptr
{
	static const type_classification classification = type_classification::MEMB_FIELD;
	typedef _T _Class::*result_type;
};

template<typename _Class, typename _RetType, typename ..._ArgTypes>
struct memb_ptr<_Class, _RetType(_ArgTypes...)>
{
	static const type_classification classification = type_classification::MEMB_FUNCTION;
	typedef _RetType (_Class::*result_type)(_ArgTypes...);

	typedef _RetType return_type;
	typedef parameters_pack<_ArgTypes...> args_type;
};

template<typename _Class, typename _RetType, typename ..._ArgTypes>
struct memb_ptr<_Class, _RetType (*)(_ArgTypes...)>
{
	static const type_classification classification = type_classification::MEMB_FUNCTION;
	typedef _RetType (_Class::*result_type)(_ArgTypes...);

	typedef _RetType return_type;
	typedef parameters_pack<_ArgTypes...> args_type;
};

#endif//ANDROMEDA_TRAITS_TYPES