#ifndef ANDROMEDA_TRAITS_TYPES
#define ANDROMEDA_TRAITS_TYPES

#include <functional>
#include <type_traits>

#include "../traits/logic.h"

namespace andromeda {
	namespace traits {
		//左值强制类型转换
		template<typename T1,typename T2>
		__attribute__((always_inline)) inline constexpr T1& cast(T2& lv)
		{
			return *(T1*)(&lv);
		}

		template<typename T1,typename T2>
		__attribute__((always_inline)) inline constexpr T1& cast(T2&& rv)
		{
			return *(T1*)(&(const T2&)rv);
		}
		//判断两个类型是否相同
		template<typename T1,typename T2>
		struct type_equal
		{
			static const bool result=false;
		};

		template<typename T1>
		struct type_equal<T1,T1>
		{
			static const bool result=true;
		};
		//将成员函数指针退化为等效的普通函数指针
		template<typename Func>
		struct __degenerate_func_impl
		{
			typedef Func result_type; //结果类型
			typedef void class_type; //如果是成员函数，则是这个类的类型，如果不是成员函数，则为void
		};
		//特化成员函数指针的退化模板
		template<typename Class,typename Func>
		struct __degenerate_func_impl<Func Class::*>
		{
			typedef Func result_type;
			typedef Class class_type;
		};

		template<typename Func>
		struct degenerate_func:public __degenerate_func_impl<Func>
		{
			using __degenerate_func_impl<Func>::result_type;
			using __degenerate_func_impl<Func>::class_type;
		};
		//得到普通函数或成员函数的返回类型，Func必须为普通函数，Class为函数所属类(实际上是依靠Func判断是否为成员函数，只有Func为成员函数时Class参数才有用)，Args必须显示指定。可以配合bind_this使用，用于取得RetType
		//传入成员函数指针类型时必须写成&Class::Func，而不能写成ClassObj.Func，因为后者中子类可以重载父类的Func函数，导致decltype无法推断成员函数的类型。由于是模拟调用，成员函数必须是外部可以调用的函数，即不能声明为private或protected
		template<typename Func,typename ...Args>
		struct __get_func_ret_type_impl
		{
			typedef decltype(std::declval<std::function<Func> >()(std::declval<Args>()...)) result_type;
			typedef void class_type;		//函数所属的类
		};
		//特化成员函数指针的返回类型模板
		template<typename Class,typename Func,typename ...Args>
		struct __get_func_ret_type_impl<Func Class::*,Args...>
		{
			typedef decltype((std::declval<Class>().*std::declval<Func Class::*>())(std::declval<Args>()...)) result_type;
			typedef Class class_type;		//函数所属的类
		};

		template<typename Func,typename ...Args>
		struct get_func_ret_type:public __get_func_ret_type_impl<Func,Args...>
		{
			using __get_func_ret_type_impl<Func,Args...>::result_type;
			using __get_func_ret_type_impl<Func,Args...>::class_type;
		};

		//包装返回值、参数、所属类以得到函数指针的类型，Class=void则视作普通函数类型，否则视作成员函数类型，函数类型转函数指针使用std::decay
		template<typename Class,typename RetType,typename ... ArgsType>
		struct func_type
		{
			typedef typename _if<type_equal<Class,void>::result,RetType (*)(ArgsType...),RetType (Class::*)(ArgsType...)>::result_type result_type;
		};

		//得到绑定了cls=this的Class成员函数std::function，如果Class=void则不绑定this，指针视作普通函数指针。用法 bind_this<>::bind(func,this)
		template<typename Class,typename RetType,typename ... ArgsType>
		struct bind_this
		{
			static std::function<RetType(ArgsType...)> bind(typename func_type<Class,RetType,ArgsType...>::result_type func,Class* cls)
			{
				return [=](ArgsType&& ... args)
				{
					return (cls->*func)(std::forward<decltype(args)>(args)...);
				};
			}
		};

		//普通函数指针的实现
		template<typename RetType,typename ... ArgsType>
		struct bind_this<void,RetType,ArgsType...>
		{
			std::function<RetType(ArgsType...)> bind(typename func_type<void,RetType,ArgsType...>::result_type func,void* cls=nullptr)
			{
				return [=](ArgsType&& ... args)
				{
					return (*func)(std::forward<decltype(args)>(args)...);
				};
			}

		};

		//std::decay的拓展，可以得到指针、引用等的原本类型，即传入T*或T**甚至更高次数的指针可以得到T
		template<bool IsPtr,typename T>
		struct __get_type_impl
		{
		};

		template<typename T>
		struct __get_type_impl<true,T*>
		{
			typedef typename __get_type_impl<std::is_pointer<typename std::decay<T>::type>::value,typename std::decay<T>::type>::result_type result_type;
		};

		template<typename T>
		struct __get_type_impl<false,T>
		{
			typedef typename std::decay<T>::type result_type;
		};

		template<typename T>
		struct get_type:public __get_type_impl<std::is_pointer<typename std::decay<T>::type>::value,typename std::decay<T>::type>
		{
			typedef typename __get_type_impl<std::is_pointer<typename std::decay<T>::type>::value,typename std::decay<T>::type>::result_type result_type;
		};

		//判断是否是基础类型，如果是则结果为true，如果不是（是类）则返回false。可以传入指针、引用等，得到的依旧是原本类型
		template<typename T>
		struct __is_primitive_type_impl
		{
			static const bool result=false;
		};

#define	DEFINE_PRIMITIVE_TYPE(T)\
		template<>\
		struct __is_primitive_type_impl<T>\
		{\
			static const bool result=true;\
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

#undef DEFINE_PRIMITIVE_TYPE

		template<typename T>
		struct is_primitive_type:public __is_primitive_type_impl<typename get_type<T>::result_type>
		{
			using __is_primitive_type_impl<typename get_type<T>::result_type>::result;
		};
		//判断T是否是类，结果实际上为!(is_primitive_type<T>)
		template<typename T>
		struct is_class
		{
			static const bool result=!is_primitive_type<T>::result;
		};

	/*判断类中是否存在某个名称的函数。
	 *用法：在文件开头def_cls_has_func(函数名)，def_cls_has_func宏应当使用ifndef-define来防止重复定义
	 *之后可通过has_func(函数名)<返回值类型,参数类型>::check<类名>::result或has_func(函数名)<返回值类型,参数类型>::check_ptr(对象指针)得知是否具有某名称的public成员函数（必须可以访问才行，否则需要加入friend class has_func(函数名)）
	 */
#define def_cls_has_func(FUNC_NAME)\
		template<typename Ret,typename ...Args>\
		struct has_func_##FUNC_NAME\
		{\
			template<typename Class,Ret (Class::*MembFunc)(Args...)=&Class::FUNC_NAME>\
			static constexpr bool check_ptr(Class* cls)\
			{\
				return true;\
			};\
			static constexpr bool check_ptr(...)\
			{\
				return false;\
			};\
			template<typename Class>\
			struct check\
			{\
				static const bool result=check_ptr((Class*)0);\
			};\
		};

#define has_func(FUNC_NAME) has_func_##FUNC_NAME

	}
}

#endif//ANDROMEDA_TRAITS_TYPES