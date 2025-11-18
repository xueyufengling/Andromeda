#ifndef ANDROMEDA_UTIL_ARRAYLIST
#define ANDROMEDA_UTIL_ARRAYLIST

#include <malloc.h>
#include <string.h>
#include <stddef.h>

namespace andromeda
{
namespace util
{
template<typename E>
class array_list
{
protected:
	size_t _capcity = 0, extend_capcity = 16, last_element_idx = -1;
	E* _elements = nullptr;

public:
	array_list() :
			array_list(8)
	{

	}

	array_list(size_t arr_length) :
			_capcity(arr_length), last_element_idx(-1), extend_capcity(arr_length), _elements((E*)malloc(sizeof(E) * arr_length))
	{
	}

	array_list(size_t arr_length, size_t arr_extend_capcity) :
			_capcity(arr_length), last_element_idx(-1), extend_capcity(arr_extend_capcity), _elements((E*)malloc(sizeof(E) * arr_length))
	{
	}

	array_list(const array_list<E>& arr) :
			_capcity(arr._capcity), last_element_idx(arr.last_element_idx), extend_capcity(arr.extend_capcity), _elements((E*)malloc(sizeof(E) * arr._capcity))
	{
		memcpy(_elements, arr._elements, sizeof(E) * arr._capcity);
	}

	array_list(array_list<E>&& arr) :
			_capcity(arr._capcity), last_element_idx(arr.last_element_idx), extend_capcity(arr.extend_capcity), _elements(arr._elements)
	{
		arr._elements = nullptr;
	}

	array_list(E* elem, size_t len) :
			_elements(elem), _capcity(len), last_element_idx(len - 1)
	{
	}

	array_list(E* elem, size_t len, size_t arr_extend_capcity) :
			_elements(elem), _capcity(len), last_element_idx(len - 1), extend_capcity(arr_extend_capcity)
	{
	}

	~array_list()
	{
		free(_elements);
	}

	inline operator E*()
	{
		return _elements;
	}

	inline void operator=(array_list& arr)
	{
		_capcity = arr._capcity;
		last_element_idx = arr.last_element_idx;
		extend_capcity = arr.extend_capcity;
		_elements = (E*)malloc(sizeof(E) * arr._capcity);
		memcpy(_elements, arr._elements, sizeof(E) * arr._capcity);
	}

	inline void operator=(array_list&& arr)
	{
		_capcity = arr._capcity;
		last_element_idx = arr.last_element_idx;
		extend_capcity = arr.extend_capcity;
		_elements = arr._elements;
		arr._elements = nullptr;
	}

	template<typename ...Args>
	E& add_placement_new(Args ... args)
	{
		if(last_element_idx >= _capcity - 1)
			extendArray(extend_capcity);
		new (_elements + (++last_element_idx)) E(args...);
		return lastElement();
	}

	inline void add(E& e)
	{
		if(last_element_idx >= _capcity - 1)
			extendArray(extend_capcity);
		*(_elements + (++last_element_idx)) = e;
	}

	inline void add(E&& e)
	{
		if(last_element_idx >= _capcity - 1)
			extendArray(extend_capcity);
		*(_elements + (++last_element_idx)) = e;
	}

	inline void add(E& e1, E& e2)
	{
		add(e1);
		add(e2);
	}

	template<typename ...Es>
	inline void add(Es& ...es)
	{
		checkExtendArray(sizeof...(Es));
		E arr[sizeof...(Es)] = {es...};
		memcpy(_elements + last_element_idx + 1, arr, sizeof(E) * (sizeof...(Es)));
		last_element_idx += sizeof...(Es);
	}

	//直接拷贝传值
	inline void addv(E e)
	{
		if(last_element_idx >= _capcity - 1)
			extendArray(extend_capcity);
		*(_elements + (++last_element_idx)) = e;
	}

	inline void addv(E e1, E e2)
	{
		add(e1);
		add(e2);
	}

	template<typename ...Es>
	inline void addv(Es ...es)
	{
		checkExtendArray(sizeof...(Es));
		E arr[sizeof...(Es)] = {es...};
		memcpy(_elements + last_element_idx + 1, arr, sizeof(E) * (sizeof...(Es)));
		last_element_idx += sizeof...(Es);
	}

	inline void add(E* es, size_t num)
	{
		checkExtendArray(num);
		memcpy(_elements + last_element_idx, es, sizeof(E) * num);
	}

	inline void add(array_list& arr_lst)
	{
		add(arr_lst._elements(), arr_lst.length());
	}

	inline void add(array_list&& arr_lst)
	{
		add(arr_lst._elements(), arr_lst.length());
	}

	inline E& remove()
	{
		return *(_elements + (last_element_idx--));
	}

	inline E* remove(size_t num)
	{
		last_element_idx -= num;
		return _elements + last_element_idx + 1;
	}

	inline E& lastElement()
	{
		return *(_elements + last_element_idx);
	}

	inline E& get(size_t index)
	{
		return *(_elements + index);
	}

	inline E& operator[](size_t index)
	{
		return *(_elements + index);
	}

	inline size_t capcity() //数组可使用的总长度，包括没有赋值的元素
	{
		return _capcity;
	}

	inline size_t lastIndex()
	{
		return last_element_idx;
	}

	inline size_t length() //数组实际使用的长度
	{
		return last_element_idx + 1;
	}

	inline size_t extendCapcity()
	{
		return extend_capcity;
	}

	inline E* elements()
	{
		return _elements;
	}

	inline void clear()
	{
		last_element_idx = -1;
	}

	//手动释放内存，调用后就不应该再对该对象进行任何操作，否则会出错
	inline void release()
	{
		free(_elements);
	}
	//将数据类型强制转换为指定类型
	template<typename T>
	inline array_list<T>& cast()
	{
		return *(array_list<T>*)this;
	}

	inline E* toArray()
	{
		size_t len = sizeof(E) * length();
		E* arr = (E*)malloc(len);
		memcpy(arr, _elements, len);
		return arr;
	}
protected:

	void extendArray(size_t extend_cap)
	{
		E* new_elem = (E*)malloc(sizeof(E) * (_capcity + extend_cap));
		memcpy(new_elem, _elements, sizeof(E) * _capcity);
		free(_elements);
		_capcity += extend_cap;
		_elements = new_elem;
	}

	void checkExtendArray(size_t add_num)
	{
		if(last_element_idx + add_num >= _capcity)
		{
			size_t extend_times = add_num / extend_capcity + 1;
			extendArray(extend_capcity * extend_times);
		}
	}
};
}
}

#endif // ANDROMEDA_UTIL_ARRAYLIST
