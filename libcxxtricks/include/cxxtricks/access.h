#ifndef _CXXTRICKS_ACCESS
#define _CXXTRICKS_ACCESS

#include <cxxtricks/macros.h>
#include <cxxtricks/types.h>

/**
 * @brief 访问标识符，仅用作占位符用以区分不同成员，任何一个成员都需要具有其独特的标识符，即定义一个类继承自该类
 */
template<typename _Class, typename _Memb>
struct __access_identifier
{
	static_assert(is_class<_Class>::value, "_Class must be a class type"); //静态断言，只有类成员可以有__access_identifier

	typedef _Class decl_class; //成员所属类
	typedef _Memb decl_type; //成员声明的类型
	typedef typename pmemb_type<_Class, _Memb>::type pmemb_type; //成员指针类型
	typedef typename eval_type<_Memb>::type eval_type; //成员求值类型，对字段而言是声明类型，对函数而言是返回类型

	static const type_classification classification = type_classification_of<_Memb _Class::*>::value; //成员分类，判断是成员字段还是成员函数

	__access_identifier() = delete; //不能直接使用__access_identifier作为标识符，必须写一个派生类继承自本类
};

//成员字段访问的实现
template<typename _AccessIdentifier>
struct __accessor_impl_base
{
	typedef typename _AccessIdentifier::pmemb_type pmemb_type;

	static pmemb_type pmemb;

	typedef typename _AccessIdentifier::decl_class decl_class;
	typedef typename eval_type<typename _AccessIdentifier::pmemb_type>::type eval_type;

	decl_class* pobj;

	__accessor_impl_base(decl_class* pobj)
	{
		this->pobj = pobj;
	}

	__accessor_impl_base(decl_class& obj)
	{
		pobj = &obj;
	}

	template<typename _Derived>
	_Derived operator=(const __accessor_impl_base <_AccessIdentifier>&) = delete;
};

template<typename _AccessIdentifier>
typename __accessor_impl_base<_AccessIdentifier>::pmemb_type __accessor_impl_base<_AccessIdentifier>::pmemb = nullptr;

/**
 * @brief 访问私有成员，若目标成员不存在则会直接抛出编译错误
 * 		  原理：在显式实例化模板给其指针类型模板参数直接赋值时，可以绕过访问修饰符直接取到成员指针，需要将这个取到的成员指针在本模板以外之处储存起来在使用。
 * 		  注意在显式实例化完成以后，因其模板参数包含了对private成员取指针，故被显示实例化的模板将无法声明或使用，否则会受到访问修饰符限制并报错。
 */
template<typename _AccessIdentifier, type_classification _Classification>
struct __accessor_impl
{
};

template<typename _AccessIdentifier>
struct __accessor_impl<_AccessIdentifier, type_classification::MEMB_FIELD> : public __accessor_impl_base<_AccessIdentifier>
{
	using typename __accessor_impl_base<_AccessIdentifier>::decl_class;
	using typename __accessor_impl_base<_AccessIdentifier>::eval_type;
	using __accessor_impl_base<_AccessIdentifier>::pmemb;
	using __accessor_impl_base<_AccessIdentifier>::pobj;

	__attribute__((always_inline)) inline __accessor_impl(decl_class* pobj) :
			__accessor_impl_base<_AccessIdentifier>(pobj)
	{
	}

	__attribute__((always_inline)) inline __accessor_impl(decl_class& obj) :
			__accessor_impl_base<_AccessIdentifier>(obj)
	{
	}

	__attribute__((always_inline)) inline operator eval_type&()
	{
		return pobj->*pmemb;
	}

	__attribute__((always_inline)) inline __accessor_impl <_AccessIdentifier, type_classification::MEMB_FIELD>& operator=(const eval_type& value)
	{
		pobj->*pmemb = value;
		return *this;
	}

	__attribute__((always_inline)) inline static eval_type& load(decl_class* pobj)
	{
		return pobj->*pmemb;
	}

	__attribute__((always_inline)) inline static eval_type& load(decl_class& obj)
	{
		return obj.*pmemb;
	}

	__attribute__((always_inline)) inline static void store(decl_class* pobj, eval_type&& value)
	{
		pobj->*pmemb = value;
	}

	__attribute__((always_inline)) inline static void store(decl_class& obj, eval_type&& value)
	{
		obj.*pmemb = value;
	}
};

//成员函数访问的实现
template<typename _AccessIdentifier>
struct __accessor_impl<_AccessIdentifier, type_classification::MEMB_FUNCTION> : public __accessor_impl_base<_AccessIdentifier>
{
	using typename __accessor_impl_base<_AccessIdentifier>::decl_class;
	using typename __accessor_impl_base<_AccessIdentifier>::eval_type;
	using __accessor_impl_base<_AccessIdentifier>::pmemb;
	using __accessor_impl_base<_AccessIdentifier>::pobj;

	__attribute__((always_inline)) inline __accessor_impl(decl_class* pobj) :
			__accessor_impl_base<_AccessIdentifier>(pobj)
	{
	}

	__attribute__((always_inline)) inline __accessor_impl(decl_class& obj) :
			__accessor_impl_base<_AccessIdentifier>(obj)
	{
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline auto operator()(_ArgTypes ... args) -> decltype((pobj->*pmemb)(args...))
	{
		return (pobj->*pmemb)(args...);
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline static auto call(decl_class* pobj, _ArgTypes ... args) -> decltype((pobj->*pmemb)(args...))
	{
		return (pobj->*pmemb)(args...);
	}

	template<typename ..._ArgTypes>
	__attribute__((always_inline)) inline static auto call(decl_class& obj, _ArgTypes ... args) -> decltype((obj.*pmemb)(args...))
	{
		return (obj.*pmemb)(args...);
	}
};

template<typename _AccessIdentifier>
struct __accessor: public __accessor_impl<_AccessIdentifier, _AccessIdentifier::classification>
{
	typedef _AccessIdentifier identifier;

	static constexpr type_classification classification = identifier::classification;

	using typename __accessor_impl<identifier, classification>::decl_class;

	__accessor(decl_class* pobj) :
			__accessor_impl<identifier, classification>(pobj)
	{
	}

	__accessor(decl_class& obj) :
			__accessor_impl<identifier, classification>(obj)
	{
	}
};

template<typename _AccessIdentifier, typename _AccessIdentifier::pmemb_type _Value>
struct __access_target
{
private:
	__access_target()
	{
		__accessor<_AccessIdentifier>::pmemb = _Value;
	}

	//静态初始化时将成员指针赋值到指定的标识符的__accessor
	static const __access_target _init_pmemb;
};

template<typename _AccessIdentifier, typename _AccessIdentifier::pmemb_type _Value>
const __access_target <_AccessIdentifier, _Value> __access_target<_AccessIdentifier, _Value>::_init_pmemb{};

/**
 * @brief 对于指定类名和成员名的accessor标识符
 */
#define access_identifier(class_name, memb_name) __cats__(4)(__access_identifier_, class_name, _, memb_name)

/**
 * @brief 对于指定类名和成员名的accessor类名
 */
#define accessor(class_name, memb_name) __accessor<access_identifier(class_name, memb_name)>

/**
 * @brief 执行取成员指针行为的类名，已经带全部模板参数，需要在前面加template class显示实例化本类执行取成员指针
 * 		  此宏用作显示实例化在类内部decl_accessor()时定义的相关类
 */
#define access_target(class_name, memb_name) __access_target<access_identifier(class_name, memb_name), &class_name::memb_name>

/**
 * @brief 用于显式实例化access_target，可以同时传入多个access_target()宏
 */
#define accessor_initialize(...) __explicit_instantiate__(__VA_ARGS__)

#define decl_accessor(class_name, memb_name, decl_type)\
	struct access_identifier(class_name, memb_name): __access_identifier<class_name, decl_type> {};

/**
 * @brief 是否存在指定名称的成员
 */
#define exist_memb(...) __va_macro__(exist_memb, __VA_ARGS__)

#define exist_memb1(memb_name) __cats__(2)(__exist_memb_, memb_name)

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
#define exist_memb_with_type(...) __va_macro__(exist_memb_with_type, __VA_ARGS__)

#define exist_memb_with_type1(memb_name) __cats__(2)(__exist_memb_with_type_, memb_name)

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
#define exist_memb_with_type_namespaced(...) __va_macro__(exist_memb_with_type_namespaced, __VA_ARGS__)

#define exist_memb_with_type_namespaced2(memb_name, _namespace) _namespace::__cats__(2)(__exist_memb_with_type_, memb_name)

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

#endif //_CXXTRICKS_ACCESS
