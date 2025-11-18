#ifndef ANDROMEDA_UTIL_LIMITCOUNTER
#define ANDROMEDA_UTIL_LIMITCOUNTER

namespace andromeda
{
namespace util
{
//设置起始值和终止值构成循环序列，则自增、自减运算结果值都落入该序列。只适用于基础数据类型
template<typename T>
class limit_counter
{
private:
	T counter = -1;

public:
	//如果_start>_end，则counter在自增的时候恒定为_start，自减的时候恒定为_end
	T _start = -1;
	T _end = -1;

	limit_counter() = default;
	limit_counter(T start_value, T end_value) :
			_start(start_value), _end(end_value), counter(start_value)
	{
	}

	__attribute__((always_inline)) inline operator T&()
	{
		return counter;
	}

	__attribute__((always_inline)) inline T& inc() //返回自增的结果值
	{
		++counter;
		if(counter < _start || counter > _end)
			counter = _start; //如果越过_end则回到_start
		return counter;
	}

	__attribute__((always_inline)) inline T& dec() //返回自减的结果值
	{
		--counter;
		if(counter < _start || counter > _end)
			counter = _end;
		return counter;
	}

	T& operator++() //前置++
	{
		return inc();
	}

	T& operator--()
	{
		return dec();
	}

	T operator++(int) //后置++
	{
		T old_counter = counter;
		++counter;
		if(counter < _start || counter > _end)
			counter = _start; //如果越过_end则回到_start
		return old_counter;
	}

	T operator--(int)
	{
		T old_counter = counter;
		--counter;
		if(counter < _start || counter > _end)
			counter = _end;
		return old_counter;
	}
};
}
}

#endif//ANDROMEDA_UTIL_LIMITCOUNTER
