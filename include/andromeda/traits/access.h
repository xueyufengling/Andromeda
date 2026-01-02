#ifndef ANDROMEDA_TRAITS_ACCESS
#define ANDROMEDA_TRAITS_ACCESS

#include "macros.h"
#include "types.h"

/**
 * @brief 访问标识符，仅用作占位符用以区分不同成员，任何一个成员都需要具有其独特的标识符，即定义一个类继承自该类
 */
template<typename _Class, typename _Memb>
struct __access_identifier
{
	typedef _Class class_type;
	typedef _Memb memb_type;
	typedef typename memb_ptr<_Class, _Memb>::result_type memb_ptr_type;

	static const type_classification classification = type_classification::MEMB_FIELD;

	__access_identifier() = delete; //不能直接使用__access_identifier作为标识符，必须写一个派生类继承自本类
};

//成员函数标识符
template<typename _Class, typename _RetType, typename ..._ArgTypes>
struct __access_identifier<_Class, _RetType(_ArgTypes...)>
{
	typedef _Class class_type;
	typedef _RetType (*memb_type)(_ArgTypes...);
	typedef typename memb_ptr<_Class, memb_type>::result_type memb_ptr_type;

	static const type_classification classification = type_classification::MEMB_FUNCTION;

	__access_identifier() = delete;
};

/**
 * @brief 访问私有成员，若目标成员不存在则会直接抛出编译错误
 * 		  原理：在显式实例化模板给其指针类型模板参数直接赋值时，成员指针类型的模板参数对private成员取指针时可以绕过访问修饰符直接取到成员指针，需要将这个取到的成员指针在本模板以外之处储存起来在使用。
 * 		  注意在显式实例化完成以后，因其模板参数包含了对private成员取指针，故被显示实例化的模板将无法声明或使用，否则会受到访问修饰符限制并报错。
 */
template<typename _AccessIdentifier, type_classification _Classification>
struct __accessor_impl
{
	static_assert(false, "parameter '_ConstexprMembPtr' for template __accessor_impl<> should be type constexpr_type<>");
};

//成员字段访问的实现
template<typename _AccessIdentifier>
struct __accessor_impl<_AccessIdentifier, type_classification::MEMB_FIELD>
{
	static typename _AccessIdentifier::memb_ptr_type memb_ptr;

	typedef typename _AccessIdentifier::class_type class_type;
	typedef typename return_type<typename _AccessIdentifier::memb_type>::result_type return_type;

	class_type* obj_ptr;

	__accessor_impl(class_type* obj_ptr)
	{
		this->obj_ptr = obj_ptr;
	}

	__accessor_impl(class_type& obj)
	{
		obj_ptr = &obj;
	}

	__attribute__((always_inline)) inline operator return_type&()
	{
		return obj_ptr->*memb_ptr;
	}

	__attribute__((always_inline)) inline static return_type& load(class_type* obj_ptr)
	{
		return obj_ptr->*memb_ptr;
	}

	__attribute__((always_inline)) inline static return_type& load(class_type& obj)
	{
		return obj.*memb_ptr;
	}

	__attribute__((always_inline)) inline static void store(class_type* obj_ptr, return_type& value)
	{
		obj_ptr->*memb_ptr = value;
	}

	__attribute__((always_inline)) inline static void store(class_type& obj, return_type& value)
	{
		obj.*memb_ptr = value;
	}
};
template<typename _AccessIdentifier>
typename _AccessIdentifier::memb_ptr_type __accessor_impl<_AccessIdentifier, type_classification::MEMB_FIELD>::memb_ptr = nullptr;

//成员函数访问的实现
template<typename _AccessIdentifier>
struct __accessor_impl<_AccessIdentifier, type_classification::MEMB_FUNCTION>
{
	static typename _AccessIdentifier::memb_ptr_type memb_ptr;

	typedef typename _AccessIdentifier::class_type class_type;
	typedef typename return_type<typename _AccessIdentifier::memb_type>::result_type return_type;

	class_type* obj_ptr;

	__accessor_impl(class_type* obj_ptr)
	{
		this->obj_ptr = obj_ptr;
	}

	__accessor_impl(class_type& obj)
	{
		obj_ptr = &obj;
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline auto operator()(_ArgTypes ... args) -> decltype((obj_ptr->*memb_ptr)(args...))
	{
		return (obj_ptr->*memb_ptr)(args...);
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline static auto call(class_type* obj_ptr, _ArgTypes ... args) -> decltype((obj_ptr->*memb_ptr)(args...))
	{
		return (obj_ptr->*memb_ptr)(args...);
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline static auto call(class_type& obj, _ArgTypes ... args) -> decltype((obj.*memb_ptr)(args...))
	{
		return (obj.*memb_ptr)(args...);
	}
};
template<typename _AccessIdentifier>
typename _AccessIdentifier::memb_ptr_type __accessor_impl<_AccessIdentifier, type_classification::MEMB_FUNCTION>::memb_ptr = nullptr;

template<typename _AccessIdentifier>
struct __accessor: __accessor_impl<_AccessIdentifier, _AccessIdentifier::classification>
{
	typedef _AccessIdentifier identifier;
	static constexpr type_classification classification = _AccessIdentifier::classification;

	typedef typename _AccessIdentifier::memb_ptr_type memb_ptr_type;
	using __accessor_impl<_AccessIdentifier, _AccessIdentifier::classification>::memb_ptr;

	using typename __accessor_impl<_AccessIdentifier, _AccessIdentifier::classification>::class_type;

	__accessor(class_type* obj_ptr) :
			__accessor_impl<_AccessIdentifier, _AccessIdentifier::classification>(obj_ptr)
	{
	}

	__accessor(class_type& obj) :
			__accessor_impl<_AccessIdentifier, _AccessIdentifier::classification>(obj)
	{
	}
};

template<typename _AccessIdentifier, typename _ConstexprMembPtr>
struct __accessor_initializer
{
	static_assert(false, "parameter '_ConstexprMembPtr' for template __accessor_initializer<> should be type constexpr_value<>");
};

template<typename _AccessIdentifier, typename _T, _T _Value>
struct __accessor_initializer<_AccessIdentifier, constexpr_value<_T, _Value> >
{
	static constexpr type_classification classification = constexpr_value<_T, _Value>::classification;

	static_assert(classification == type_classification::MEMB_FIELD || classification == type_classification::MEMB_FUNCTION, "parameter '_ConstexprMembPtr' for template __accessor_initializer<> should be member field or member function");

private:
	//程序初始化时取成员指针并赋值
	__attribute__((constructor)) static void init()
	{
		__accessor<_AccessIdentifier>::memb_ptr = _Value;
	}
};

/**
 * @brief 对于指定类名和成员名的accessor标识符
 */
#define access_identifier(class_name, memb_name) __cat_4__(__access_identifier_, class_name, _, memb_name)

/**
 * @brief 对于指定类名和成员名的accessor类名
 */
#define accessor(class_name, memb_name) __accessor<access_identifier(class_name, memb_name)>

/**
 * @brief 用于显式实例化accessor_initializer
 */
#define accessor_initialize(...) template class __VA_ARGS__

/**
 * @brief 执行取成员指针行为的类名，已经带全部模板参数，需要在前面加template class显示实例化本类执行取成员指针
 * 		  此宏用作显示实例化在类内部decl_accessor()时定义的相关类
 */
#define accessor_initializer(class_name, memb_name) __accessor_initializer<access_identifier(class_name, memb_name), constexpr_val(&class_name::memb_name)>

#define decl_accessor(class_name, memb_name, memb_type)\
	struct access_identifier(class_name, memb_name): __access_identifier<class_name, memb_type> {}

/**
 * @brief 是否存在指定名称的成员
 */
#define exist_memb(...) __macro_with_params__(exist_memb, __VA_ARGS__)

#define exist_memb1(memb_name) __cat__(__exist_memb_, memb_name)

#define exist_memb2(class_name, memb_name) exist_memb1(memb_name)<class_name>

/*
 * @brief 判断类中是否存在某个名称的成员，即具有指定名称的字段、函数、类型均可被判定为具有该成员，绕过访问权限。
 *		  用法：在文件开头decl_exist_memb(成员名)，并使用exist_memb(类名, 成员名)::result得到结果。
 *		  原理：
 *		  在模板参数推导过程中，对存在歧义的成员取成员取成员指针会不是硬错误，可以根据SFINAE机制不实例化该模板函数，转而调用__check(...)函数。注意如果在模板默认值而非推导中对不存在的成员取成员指针时会报错有歧义。
 *		  需要构造一个继承自被检测目标类(_Class)并继承自另一个具有public权限的与被检测目标同名的成员的类(__dummy_decl，此时两个歧义成员都是继承而来的同一层级，不会互相覆盖)的伪造类(__dummy_class)，这样伪造类就具有一个或两个相同名称的成员，如果有两个则产生歧义。该方案缺陷是无法检测ubion和final类，因为它们不能被继承。
 *		  因此，不能直接对伪造类取成员指针，而应该将伪造类作为模板参数，对这个模板参数取成员指针，此时产生歧义就是在参数推导过程中取指针失败，可以通过SFINAE机制解决而非抛出编译错误。
 */
#define decl_exist_memb_intl(memb_name)\
		template<typename _Class>\
		struct exist_memb(memb_name)\
		{\
		private:\
			exist_memb(memb_name)() = delete;\
			struct __dummy_decl\
			{\
				char memb_name;\
			};\
			struct __dummy_class: _Class, __dummy_decl\
			{\
			};\
			template<typename _Dummy>\
			inline static constexpr bool __check(decltype(&_Dummy::memb_name))\
			{\
				return false;\
			}\
			template<typename _Dummy>\
			inline static constexpr bool __check(...)\
			{\
				return true;\
			}\
		public:\
			static const bool result = __check<__dummy_class>(nullptr);\
		};

#define decl_exist_memb(...) __repeat_each__(decl_exist_memb_intl, ##__VA_ARGS__)

/**
 * @brief 是否存在指定名称和类型的成员
 */
#define exist_memb_with_type(...) __macro_with_params__(exist_memb_with_type, __VA_ARGS__)

#define exist_memb_with_type1(memb_name) __cat__(__exist_memb_with_type_, memb_name)

#define exist_memb_with_type3(class_name, type, memb_name) exist_memb_with_type1(memb_name)<class_name, type>

/**
 * @brief 判断类中是否存在某个名称和类型的成员，需要public访问权限或将本类添加为目标类的friend class。
 * 		  __check()必须有个区别于_Class的类型参数_DeducedClass，实际调用时_DeducedClass被替换成_Class，如果目标不存在则属于推导错误，可以使用SFINAE机制。如果直接&_Class::memb_name由于_Class在推导阶段前就已知，目标不存在导致硬错误编译失败
 * 		  注：在gcc version 14.1.0 (Rev3, Built by MSYS2 project)版本中，检查类中存在但类型不匹配的成员时，__check<>()推导过程中抛出编译器内部错误：internal compiler error: in instantiate_type, at cp/class.cc:9115
 */
#define decl_exist_memb_with_type_intl(memb_name)\
		template<typename _Class, typename _T>\
		struct exist_memb_with_type(memb_name)\
		{\
		private:\
			exist_memb_with_type(memb_name)() = delete;\
			template<typename _DeducedClass = _Class, _T _DeducedClass::*_MembPtr = &_DeducedClass::memb_name>\
			static constexpr bool __check(int)\
			{\
				return true;\
			}\
			static constexpr bool __check(...)\
			{\
				return false;\
			}\
		public:\
			static const bool result = __check(int());\
		};

/**
 * @brief 带命名空间的类型
 */
#define exist_memb_with_type_namespaced(...) __macro_with_params__(exist_memb_with_type_namespaced, __VA_ARGS__)

#define exist_memb_with_type_namespaced2(memb_name, _namespace) _namespace::__cat__(__exist_memb_with_type_, memb_name)

#define exist_memb_with_type_namespaced4(class_name, type, memb_name, _namespace) exist_memb_with_type_namespaced2(memb_name, _namespace)<class_name, type>

/*
 * @brief 判断类中是否存在某个名称的函数，要求是public访问权限或者本类是目标类的friend class。
 * @params ... 名称列表
 */
#define decl_exist_memb_with_type(...) __repeat_each__(decl_exist_memb_with_type_intl, ##__VA_ARGS__)

#define __exist_memb_with_type_template_params__() typename, typename

/**
 * @brief 在要检测的目标类中使用，将检测类全部设置为目标类的friend class。
 * @param _namespace_macro 为空参数宏的宏名，指定检测类所属的命名空间，全局命名空间则留空，例如
 * 		  #define detect_namespace() andromeda::common
 * 		  注：必须指定命名空间的原因是，如果检测类Detect的命名空间A与目标类的命名空间B不同，那么不带命名空间所声明的friend class实际上是B::Detect，而不是A::Detect，将导致设置友元失败
 */
#define enable_exist_memb_with_type(_namespace_macro, ...) __template_friend_classes__(__exist_memb_with_type_template_params__, __op_each_extras__(exist_memb_with_type_namespaced, _namespace_macro, __VA_ARGS__))

#endif //ANDROMEDA_TRAITS_ACCESS
