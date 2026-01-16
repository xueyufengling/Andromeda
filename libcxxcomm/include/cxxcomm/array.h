#ifndef _CXXCOMM_ARRAY
#define _CXXCOMM_ARRAY

#include <malloc.h>
#include <string.h>
#include <stddef.h>

namespace cxxcomm
{
/**
 * @brief 将变长参数列表打包成指定类型的数组
 */
template<typename T, typename ...Args>
inline T* pack_c_array(Args ... args)
{
	return new T[sizeof...(args)]{args...};
}

/**
 * @brief 轻量级可变长数组模板
 */
template<typename E>
class array
{
public:
	static size_t default_init_capcity;
	static size_t default_extend_capcity;

	static constexpr size_t no_element_idx = -1;
	static constexpr size_t capacity_placeholder = -2;

protected:
	size_t capcity = 0, //当前容量
			extend_capcity = default_extend_capcity; //每次拓展时的新增容量

	E* elements = nullptr;

private:
	size_t end_pos = no_element_idx; //末尾元素索引
	bool self_alloc = true; //本数组的内存是否是自己分配的

	inline void _add()
	{
	}

	template<typename A>
	inline void _add(A e)
	{
		*(elements + (++end_pos)) = e;
	}

	inline void _add(array<E>& arr)
	{
		add(arr);
	}

	template<typename A, typename ...Es>
	inline void _add(A e, Es ...es)
	{
		_add(e);
		_add(es...);
	}

protected:
	void extend(size_t extend_cap)
	{
		E* new_elem = (E*)malloc(sizeof(E) * (capcity + extend_cap));
		if(elements) //如果end_pos不为-1且elements为nullptr。则会导致新分配的elements内存的前end_pos索引处的数据全为内存垃圾数据
			memcpy(new_elem, elements, sizeof(E) * capcity);
		if(self_alloc)
			free(elements);
		capcity += extend_cap;
		elements = new_elem;
		self_alloc = true; //扩容后一定是自己分配的内存，要负责回收
	}

	__attribute__((always_inline)) inline void extend_if_need(size_t add_num = 1)
	{
		if(!elements || end_pos + add_num >= capcity) //elements为nullptr或容量不足时需要扩容
		{
			extend(extend_capcity * (add_num / extend_capcity + 1));
		}
	}

	void extend_to(size_t num)
	{
		E* new_elem = (E*)malloc(sizeof(E) * num);
		size_t new_num = capcity > num ? num : capcity; //如果容量缩小就截断数组后部数据
		if(elements)
			memcpy(new_elem, elements, sizeof(E) * new_num);
		if(self_alloc)
			free(elements);
		capcity = num;
		elements = new_elem;
		self_alloc = true; //扩容后一定是自己分配的内存，要负责回收
	}

public:
	array() = default;

	array(size_t init_capcity, size_t extend_capcity = default_extend_capcity) :
			capcity(init_capcity), extend_capcity(extend_capcity), self_alloc(true)
	{
		elements = (E*)malloc(sizeof(E) * init_capcity);
	}

	~array()
	{
		if(self_alloc)
			free(elements);
	}

	array(const array<E>& arr) :
			capcity(arr.capcity), extend_capcity(arr.extend_capcity), end_pos(arr.end_pos), self_alloc(true)
	{
		size_t cpy_len = sizeof(E) * arr.capcity;
		elements = (E*)malloc(cpy_len);
		memcpy(elements, arr.elements, cpy_len);
	}

	array(array<E>&& arr) :
			capcity(arr.capcity), extend_capcity(arr.extend_capcity), end_pos(arr.end_pos), elements(arr.elements), self_alloc(arr.self_alloc)
	{
		arr.elements = nullptr;
	}

	template<int N>
	array(E elem[N]) :
			elements(elem), capcity(N), end_pos(N - 1), self_alloc(false)
	{
	}

	array(E* elem, size_t len, size_t extend_capcity = default_extend_capcity) :
			elements(elem), capcity(len), end_pos(len - 1), extend_capcity(extend_capcity), self_alloc(false)
	{
	}

	/**
	 * @brief 对内存区域调用T的构造函数。必须手动调用。
	 * 		  如果T不是基本类型，那么新分配的内存区域全是垃圾数据构成的对象，对其赋值时有可能因为拷贝/移动构造函数中操作垃圾数据引发崩溃
	 * @param start_idx 以E为单位的起始坐标
	 * @param end_idx 以E为单位的终止坐标
	 */
	template<typename T = E, typename ...Args>
	array& construct(size_t start_idx = 0, size_t end_idx = capacity_placeholder, Args ...args)
	{
		end_idx = (end_idx == capacity_placeholder) ? capcity : end_idx;
		size_t length = end_idx - start_idx + 1;
		T* dest = (T*)(elements + start_idx);
		for(size_t idx = 0; idx < length; ++idx)
		{
			new (dest + idx) T(args...);
		}
		return *this;
	}

	inline operator E*()
	{
		return elements;
	}

	inline operator void*()
	{
		return elements;
	}

	inline operator bool()
	{
		return elements;
	}

	/**
	 * 浅拷贝
	 */
	inline array& operator=(array& arr)
	{
		capcity = arr.capcity;
		end_pos = arr.end_pos;
		extend_capcity = arr.extend_capcity;
		elements = arr.elements;
		self_alloc = false;
		return *this;
	}

	inline array& operator=(array&& arr)
	{
		capcity = arr.capcity;
		end_pos = arr.end_pos;
		extend_capcity = arr.extend_capcity;
		elements = arr.elements;
		self_alloc = arr.self_alloc;
		arr.elements = nullptr;
		return *this;
	}

	template<typename ...Args>
	E& add_placement_new(Args ... args)
	{
		extend_if_need();
		new (elements + (++end_pos)) E(args...);
		return last();
	}

	inline void add(E& e)
	{
		extend_if_need();
		*(elements + (++end_pos)) = e;
	}

	inline void add(E&& e)
	{
		extend_if_need();
		*(elements + (++end_pos)) = e;
	}

	template<typename ...Es>
	inline void add(Es& ...es)
	{
		extend_if_need(sizeof...(Es));
		_add(es...);
	}

	template<typename ...Es>
	inline void add(Es&& ...es)
	{
		extend_if_need(sizeof...(Es));
		_add(es...);
	}

	inline void add(E* es, size_t num)
	{
		extend_if_need(num);
		memcpy(elements + end_pos, es, sizeof(E) * num);
	}

	inline void add(array& arr)
	{
		add(arr.elements, arr.length());
	}

	inline void add(array&& arr)
	{
		add(arr.elements, arr.length());
	}

	/**
	 * @brief 移除最后一个元素并返回其值
	 */
	inline E& remove()
	{
		return *(elements + (end_pos--));
	}

	inline E* remove(size_t num)
	{
		end_pos -= num;
		return elements + end_pos + 1;
	}

	inline E& last()
	{
		return *(elements + end_pos);
	}

	inline E& operator[](size_t index)
	{
		return *(elements + index);
	}

	inline size_t current_capcity() //数组可使用的总长度，包括没有赋值的元素
	{
		return capcity;
	}

	inline size_t length() //数组实际使用的长度
	{
		return end_pos + 1;
	}

	inline size_t current_extend_capcity()
	{
		return extend_capcity;
	}

	inline void set_extend_capcity(size_t cap)
	{
		extend_capcity = cap;
	}

	inline void clear()
	{
		end_pos = no_element_idx;
	}

	/**
	 * @brief 手动清空全部数据并释放内存
	 */
	inline void release()
	{
		free(elements);
		capcity = default_init_capcity;
		extend_capcity = default_extend_capcity;
		end_pos = no_element_idx;
		elements = nullptr;
		self_alloc = true;
	}

	/**
	 * @brief 提前预留内存，如果不足则重新分配
	 */
	inline void reserve(size_t size)
	{
		if(!elements || size > capcity) //elements为nullptr或容量不足时需要扩容
		{
			extend_to(size);
		}
	}

	/**
	 * @brief 将数据类型强制转换为指定类型
	 */
	template<typename T>
	inline array<T>& cast()
	{
		return *(array<T>*)this;
	}

	inline E* c_array()
	{
		size_t len = sizeof(E) * length();
		E* arr = (E*)malloc(len);
		memcpy(arr, elements, len);
		return arr;
	}
};

//默认初始容量，可运行时修改
template<typename E>
size_t array<E>::default_init_capcity = 1024;

template<typename E>
size_t array<E>::default_extend_capcity = 1024;

using buffer = array<unsigned char>;

}

#endif // _CXXCOMM_ARRAY
