#ifndef ANDROMEDA_TRAITS_ACCESS
#define ANDROMEDA_TRAITS_ACCESS

#include "types.h"

enum __access_classification : int
{
	FIELD, FUNCTION
};

/**
 * @brief 访问标识符，仅用作占位符用以区分不同成员，任何一个成员都需要具有其独特的标识符，即定义一个类继承自该类
 */
template<typename Class, typename Memb>
struct __access_identifier
{
	typedef Class class_type;
	typedef Memb memb_type;
	typedef typename memb_ptr<Class, Memb>::result_type memb_ptr_type;

	static const __access_classification classification = __access_classification::FIELD;

	__access_identifier() = delete; //不能直接使用__access_identifier作为标识符，必须写一个派生类继承自本类
};

//成员函数标识符
template<typename Class, typename Ret, typename ...Args>
struct __access_identifier<Class, Ret(Args...)>
{
	typedef Class class_type;
	typedef typename memb_ptr<Class, Ret(Args...)>::result_type memb_ptr_type;
	typedef memb_ptr_type memb_type;

	static const __access_classification classification = __access_classification::FUNCTION;

	__access_identifier() = delete;
};

/**
 * @brief 访问私有成员
 * 		  原理：模板参数中对private成员取指针时可以绕过访问修饰符直接取到成员指针，但取到的成员指针值不能直接在本类的内部使用，否则也会报错
 */
template<typename AccessIdentifier>
struct __access_entry_base
{
	static typename AccessIdentifier::memb_ptr_type memb_ptr; //不能直接接收private成员指针的值，否则抛出编译错误 private within this context

	typename AccessIdentifier::class_type* obj_ptr;
};
template<typename AccessIdentifier>
typename AccessIdentifier::memb_ptr_type __access_entry_base<AccessIdentifier>::memb_ptr = nullptr;

template<typename AccessIdentifier, __access_classification Classification>
struct __access_entry
{
	static_assert(true, "__access_entry unknown classification");
};

template<typename AccessIdentifier>
struct __access_entry<AccessIdentifier, __access_classification::FIELD> : public __access_entry_base<AccessIdentifier>
{
	using __access_entry_base<AccessIdentifier>::memb_ptr;
	using __access_entry_base<AccessIdentifier>::obj_ptr;

	__attribute__((always_inline)) inline operator typename AccessIdentifier::memb_type&()
	{
		return obj_ptr->*memb_ptr;
	}
};

template<typename AccessIdentifier>
struct __access_entry<AccessIdentifier, __access_classification::FUNCTION> : public __access_entry_base<AccessIdentifier>
{
	using __access_entry_base<AccessIdentifier>::memb_ptr;
	using __access_entry_base<AccessIdentifier>::obj_ptr;

	template<typename ...Args>
	__attribute__((always_inline)) inline auto operator()(Args ... args) -> decltype((obj_ptr->*memb_ptr)(args...))
	{
		return (obj_ptr->*memb_ptr)(args...);
	}
};

/**
 * @brief 访问成员的接口
 */
template<typename AccessIdentifier>
struct __access: public __access_entry<AccessIdentifier, AccessIdentifier::classification>
{
	using __access_entry_base<AccessIdentifier>::memb_ptr;
	using __access_entry_base<AccessIdentifier>::obj_ptr;

	static const __access_classification classification = AccessIdentifier::classification;

	__access(typename AccessIdentifier::class_type* obj)
	{
		obj_ptr = obj;
	}

	__access(typename AccessIdentifier::class_type& obj)
	{
		obj_ptr = &obj;
	}
};

/**
 * @brief 利用模板参数值取成员指针地址，并将其赋值给__access_entry_base对象
 */
template<typename AccessIdentifier, typename AccessIdentifier::memb_ptr_type MembPtr>
struct __access_fetch
{
	typedef __access <AccessIdentifier> result_type;

	static_assert(MembPtr != nullptr, "__access fetch member pointer failed, no declaration found");

private:
	//程序初始化时取成员指针并赋值
	__attribute__((constructor(101))) static void __fetch()
	{
		result_type::memb_ptr = MembPtr;
	}
};

#define access_identifier(class_name, memb_name) __access_identifier_##class_name##_##memb_name

#define enable_access(class_name, memb_name, memb_type)\
		struct access_identifier(class_name, memb_name): __access_identifier<class_name, memb_type>\
		{\
		};\
		template class __access_fetch<access_identifier(class_name, memb_name), &class_name::memb_name>

#define accessor(class_name, memb_name) __access<access_identifier(class_name, memb_name)>

/*
 * @brief 判断类中是否存在某个名称的函数。
 *		  用法：在文件开头decl_exist_memb_func(函数名, 返回类型, 参数类型列表)，decl_exist_memb_func宏应当使用ifndef-define来防止重复定义
 *		  原理：使用__access访问private成员，并使用SFINAE机制在&DummyClass::func_name失败时可判断原类不存在同签名的成员函数
 */
#define decl_exist_memb_func(func_name, ret_type, ...)\
		template<typename Class, typename Ret,typename ...Args>\
		struct __exist_memb_func_##func_name\
		{\
		private:\
			struct __dummy_func_decl\
			{\
				char func_name;\
			};\
 			struct __dummy_class: protected __dummy_func_decl, protected Class\
			{\
				friend class __exist_memb_func_##func_name<Class, Ret, Args...>;\
			};\
			template<typename = decltype(&__dummy_class::func_name)>\
			inline static constexpr bool __check()\
			{\
				return true;\
			};\
			inline static constexpr bool __check(...)\
			{\
				return false;\
			};\
		public:\
			static const bool result = __check();\
		};

#define exist_memb_func(class_name, func_name, ret_type, ...) __exist_memb_func_##func_name<class_name, ret_type, ##__VA_ARGS__>

#endif //ANDROMEDA_TRAITS_ACCESS
