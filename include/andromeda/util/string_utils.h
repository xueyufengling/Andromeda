#ifndef ANDROMEDA_UTIL_STRINGUTILS
#define ANDROMEDA_UTIL_STRINGUTILS

#include "array_list.h"

namespace andromeda
{
namespace util
{
class split_strings;

split_strings split(const char* str, const char* delim); //编译器将优化，直接在调用处构造返回值。释放返回值中的所有字符串时只需调用free(arr[0])！

class split_strings : public array_list<const char*>
{
	friend split_strings andromeda::util::split(const char* str, const char* delim);

private:
	using array_list<const char*>::array_list;
	using array_list<const char*>::release;
	split_strings() = default;

public:
	~split_strings() = default;
};

void pack_bits_to_bytes(unsigned char* bits, int bits_start_pos, unsigned char* bytes, int bytes_start_pos, long int bits_length);

bool parse_bool(const char* value); //当value为"true"时返回true，其他时候返回false

const char* str_cpy(const char* str); //拷贝字符串
const char* str_arr_join(const char** str_arr, int count); //把含有count个字符串的数组合并为一个字符串

const char* str_join(const char* str1, const char* str2);

const char* str_join(const char* str1, char ch);
const char* str_join(char ch, const char* str1);

__attribute__((always_inline)) inline const char* str_join(const char* str)
{
	return str == nullptr ? "" : str;
}

__attribute__((always_inline)) inline const char* str_join(bool b)
{
	return b ? "true" : "false";
}

__attribute__((always_inline)) inline const char* str_join(char ch)
{
	return (const char*)new char[1]{ch};
}

template<typename T>
__attribute__((always_inline)) inline void* memasgn(void* ptr, T&& value) //浅拷贝对象到给定指针
{
	return memcpy(ptr, &value, sizeof(T));
}

template<typename T>
__attribute__((always_inline)) void* memasgn(void* ptr, T& value)
{
	return memcpy(ptr, &value, sizeof(T));
}

inline char int_0_to_9_to_char(int num)
{
	return (char)(num + 48);
}

const char* int_to_string_dec(long long int num);

template<typename T>
inline const char* int_to_string_dec(T num)
{
	return int_to_string_dec((long long int)num);
}

inline int char_to_int_0_to_9(char ch)
{
	return (int)(ch - 48);
}

__attribute__((always_inline)) inline const char* str_join(int num)
{
	return int_to_string_dec(num);
}

__attribute__((always_inline)) inline const char* str_join(unsigned int num)
{
	return int_to_string_dec(num);
}

inline const char* str_join(const char* str1, int num)
{
	return str_join(str1, str_join(num));
}

inline const char* str_join(int num, const char* str1)
{
	return str_join(str_join(num), str1);
}

inline const char* str_join(const char* str1, unsigned int num)
{
	return str_join(str1, str_join(num));
}

inline const char* str_join(unsigned int num, const char* str1)
{
	return str_join(str_join(num), str1);
}

inline const char* str_join(bool b, const char* str1)
{
	return str_join(str_join(b), str1);
}

inline const char* str_join(const char* str1, bool b)
{
	return str_join(str1, str_join(b));
}

inline const char* str_join(char* str1, const char* str2)
{
	return str_join((const char*)str1, str2);
}

inline const char* str_join(const char* str1, char* str2)
{
	return str_join(str1, (const char*)str2);
}

template<typename T1>
const char* str_join(T1 str1)
{
	return str_join(str1);
}
template<typename T1, typename T2>
const char* str_join(T1 str1, T2 str2)
{
	return str_join(str1, str2);
}
template<typename T, typename ...Ts>
const char* str_join(T str, Ts ...strs)
{
	return str_join(str, str_join(strs...));
}

//如果是字母则返回大写形式，否则返回本身
inline char uppercase(char ch)
{
	return ch >= 'a' && ch <= 'z' ? ch - 32 : ch;
}

inline char lowercase(char ch)
{
	return ch >= 'A' && ch <= 'Z' ? ch + 32 : ch;
}
}
}

#endif // ANDROMEDA_UTIL_STRINGUTILS
