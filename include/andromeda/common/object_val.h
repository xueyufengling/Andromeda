#ifndef ANDROMEDA_COMMON_OBJECTVAL
#define ANDROMEDA_COMMON_OBJECTVAL

#include <stdlib.h>
#include "../traits/types.h"

namespace andromeda
{
namespace common
{
/**
 * @brief 包装基本类型值和对象
 */
class object_val
{
private:
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

	object_val() = default;

	template<typename T>
	inline object_val(const T& value)
	{
		store(value);
	}

	template<typename T>
	inline operator T&() const
	{
		return fetch<T>();
	}

	inline object_val& operator=(const object_val& other)
	{
		this->val = other.val;
		return *this;
	}

	inline bool operator==(const object_val& other) const
	{
		return this->val == other.val;
	}

	inline bool operator!=(const object_val& other) const
	{
		return this->val != other.val;
	}

	/**
	 * @brief 将本变量解释为指定类型的基本类型或指针值值，或指针指向的对象
	 */
	template<typename T>
	__attribute__((always_inline)) inline T& fetch() const
	{
		if(is_directly_stored<T>())
			return *(T*)(&val);
		else
			return *(T*)ptr;
	}

	/**
	 * @brief 储存基本类型、指针值或对象的指针
	 */
	template<typename T>
	__attribute__((always_inline)) inline void store(const T& value)
	{
		if(is_directly_stored(value))
			*(T*)(&val) = value; //基本类型则直接存值
		else
			ptr = (void*)&value; //非基本类型则存指针
	}

	template<typename T>
	inline object_val& operator=(const T& val)
	{
		store(val);
		return *this;
	}

	template<typename T>
	inline object_val& operator=(T&& val)
	{
		abort(); //如果是X值，则存在X值消亡后ptr变成悬空指针的危险，故直接终止程序
	}

	template<typename T>
	__attribute__((always_inline)) inline bool equals(object_val other)
	{
		return this->fetch<T>() == other.fetch<T>();
	}

	/**
	 * @brief 从容器内取出指定索引的指定类型的值
	 */
	template<typename T, typename Container, typename Idx>
	__attribute__((always_inline)) inline static T& fetch(Container container, Idx idx)
	{
		object_val* ptr = (object_val*)&container[idx];
		return ptr->fetch<T>();
	}
};

}
}

#endif //ANDROMEDA_COMMON_OBJECTVAL
