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
 * @brief 访问私有成员，若目标成员不存在则会直接抛出编译错误
 * 		  原理：在显式实例化模板给其指针类型模板参数直接赋值时，成员指针类型的模板参数对private成员取指针时可以绕过访问修饰符直接取到成员指针，需要将这个取到的成员指针储存起来供使用。注意在声明模板时如果给模板参数指定默认值为取private成员地址，则依然会受到访问修饰符限制并报错
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
	static_assert(false, "__access_entry unknown classification");
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

	static_assert(MembPtr != nullptr, "__access fetch member pointer failed, cannot be nullptr");

private:
	//程序初始化时取成员指针并赋值
	__attribute__((constructor(0))) static void __fetch()
	{
		result_type::memb_ptr = MembPtr;
	}
};

#define access_identifier(class_name, memb_name) __access_identifier_##class_name##_##memb_name

#define decl_access(class_name, memb_name, memb_type)\
		struct access_identifier(class_name, memb_name): __access_identifier<class_name, memb_type>\
		{\
		}

/**
 * @brief 执行取成员指针行为的类名，已经带全部模板参数，需要在前面加template class显示实例化本类执行取成员指针
 * 		  此宏用作显示实例化在类内部decl_access()时定义的相关类
 */
#define access_fetcher(class_name, memb_name, memb_type) __access_fetch<access_identifier(class_name, memb_name), &class_name::memb_name>

#define enable_access(class_name, memb_name, memb_type) template class access_fetcher(class_name, memb_name, memb_type)

#define accessor(class_name, memb_name) __access<access_identifier(class_name, memb_name)>

#define exist_memb_identifier_name(memb_name) __exist_memb_##memb_name
/*
 * @brief 判断类中是否存在某个名称的成员，字段、函数均可检测。
 *		  用法：在文件开头decl_exist_memb(成员名)，decl_exist_memb宏应当使用ifndef-define来防止重复定义
 *		  原理：
 *		  在模板参数推导过程中，对存在歧义的成员取成员取成员指针会不是硬错误，可以根据SFINAE机制不实例化该模板函数，转而调用__check(...)函数。注意如果在模板默认值而非推导中对不存在的成员取成员指针时会报错有歧义。
 *		  需要构造一个继承自被检测目标类(Class)并继承自另一个具有public权限的与被检测目标同名的成员的类(__dummy_decl，此时两个歧义成员都是继承而来的同一层级，不会互相覆盖)的伪造类(__dummy_class)，这样伪造类就具有一个或两个相同名称的成员，如果有两个则产生歧义。该方案缺陷是无法检测ubion和final类，因为它们不能被继承。
 *		  因此，不能直接对伪造类取成员指针，而应该将伪造类作为模板参数，对这个模板参数取成员指针，此时产生歧义就是在参数推导过程中取指针失败，可以通过SFINAE机制解决而非抛出编译错误。
 */
#define decl_exist_memb(memb_name)\
		template<typename Class>\
		struct exist_memb_identifier_name(memb_name)\
		{\
		private:\
			struct __dummy_decl\
			{\
				char memb_name;\
			};\
			struct __dummy_class: __dummy_decl, Class\
			{\
			};\
			template<typename Dummy>\
			inline static constexpr bool __check(decltype(&Dummy::memb_name))\
			{\
				return false;\
			};\
			template<typename Dummy>\
			inline static constexpr bool __check(...)\
			{\
				return true;\
			};\
		public:\
			static const bool result = __check<__dummy_class>(nullptr);\
		}

#define exist_memb(class_name, memb_name) exist_memb_identifier_name(memb_name)<class_name>

#define exist_memb_field(class_name, memb_name, memb_type) exist_memb_field_identifier_name(memb_name)<class_name, memb_type>

#define exist_memb_field_identifier_name(memb_name) __exist_memb_field_##memb_name
/**
 * @brief 查询是否有指定名称、指定类型的成员字段
 * 		  __type_check仅接收目标类型才能实例化，如果成员指针的类型与目标类型不同则__type_check实例化失败，进而引发__check()实例化失败，触发SFINAE机制
 */

#endif //ANDROMEDA_TRAITS_ACCESS
