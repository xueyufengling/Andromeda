#ifndef ANDROMEDA_INTERNAL_OBJECT
#define ANDROMEDA_INTERNAL_OBJECT

#include <stdint.h>
#include <typeinfo>
#include "../traits/types.h"

/**
 * @brief 包装基本类型值和对象
 */
class object
{
private:
	std::type_info const* _type_info = nullptr;
	union
	{
		uint64_t val = 0; //基本类型或指针值
		void* ptr; //对象指针
	};

public:
	/**
	 * @brief 判断是否是直接存值的量，包括基本类型和指针
	 */
	template<typename T>
	__attribute__((always_inline)) inline static constexpr bool is_directly_stored(void)
	{
		return is_ptr<T>::result && is_primitive<T>::result;
	}

	template<typename T>
	__attribute__((always_inline)) inline static constexpr bool is_directly_stored(T val)
	{
		return is_directly_stored<T>();
	}

	object() = default;

	template<typename T>
	inline object(const T& value)
	{
		store(value);
	}

	inline constexpr object(const object& other) :
			_type_info(other._type_info), val(other.val)
	{
	}

	inline object(object&& other)
	{
		this->val = other.val;
		this->_type_info = other._type_info;
	}

	template<typename T>
	inline operator T&() const
	{
		return interpret<T>();
	}

	inline operator const std::type_info&() const
	{
		return *_type_info;
	}

	inline bool operator==(const object& other) const
	{
		return this->val == other.val;
	}

	inline bool operator!=(const object& other) const
	{
		return this->val != other.val;
	}

	/**
	 * @brief 将本变量解释为指定类型的基本类型或指针值值，或指针指向的对象
	 */
	template<typename T>
	__attribute__((always_inline)) inline T& interpret() const
	{
		if(is_directly_stored<T>())
			return *(T*)(&val);
		else
			return *(T*)ptr;
	}

	__attribute__((always_inline)) inline const std::type_info& type() const
	{
		return *_type_info;
	}

	/**
	 * @brief 储存基本类型、指针值或对象的指针
	 */
	template<typename T>
	__attribute__((always_inline)) inline void store(const T& value)
	{
		this->_type_info = &typeid(value);
		if(is_directly_stored(value))
			*(T*)(&val) = value; //基本类型则直接存值
		else
			ptr = (void*)&value; //非基本类型则存指针
	}

	template<typename T>
	inline object& operator=(const T& val)
	{
		store(val);
		return *this;
	}

	inline object& operator=(const object& other)
	{
		this->val = other.val;
		this->_type_info = other._type_info;
		return *this;
	}

	/**
	 * @brief 如果是X值，则存在X值消亡后ptr变成悬空指针的危险，故只保留object的X值赋值操作
	 */
	inline object& operator=(object&& other)
	{
		this->val = other.val;
		this->_type_info = other._type_info;
		return *this;
	}

	template<typename T>
	__attribute__((always_inline)) inline bool equals(const object& other)
	{
		return this->interpret<T>() == other.interpret<T>();
	}

	/**
	 * @brief 从容器内取出指定索引的指定类型的值
	 */
	template<typename T, typename Container, typename Idx>
	__attribute__((always_inline)) inline static T& interpret(Container container, Idx idx)
	{
		object* ptr = (object*)&container[idx];
		return ptr->interpret<T>();
	}
};

#endif //ANDROMEDA_INTERNAL_OBJECT
