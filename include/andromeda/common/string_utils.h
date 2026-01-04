#ifndef ANDROMEDA_COMMON_STRINGUTILS
#define ANDROMEDA_COMMON_STRINGUTILS

#include "../internal/signals.h"
#include "../io/paths.h"
#include "../traits/macros.h"
#include "array.h"

#ifdef __FILE__
#define __FILE_STRING__ __FILE__
#define __FILENAME_STRING__ (andromeda::io::get_filename(__FILE_STRING__))
#else
#define __FILE_STRING__ "<__FILE__ UNDEFINED>"
#define __FILENAME_STRING__ "<__FILE__ UNDEFINED>"
#endif

#ifdef __LINE__
#define __LINE_STRING__ __str__(__LINE__)
#else
#define __LINE_STRING__ "<__LINE__ UNDEFINED>"
#endif

#include <sstream>
#include <iomanip>
#include <string>
#include <chrono>
#include <ctime>

template<typename T>
std::string to_string(const T& value);

#define StdToString(type)\
template<>\
__attribute__((always_inline)) inline std::string to_string(const type& value)\
{\
	return std::to_string(value);\
}

__repeat_each__(StdToString,
		double,
		float,
		int,
		long double,
		long int,
		long long int,
		unsigned int,
		unsigned long int,
		unsigned long long int,
		short,
		unsigned short)

#undef StdToString

template<>
__attribute__((always_inline)) inline std::string to_string(const char& value)
{
	char str[2] = {value, '\0'};
	return std::string(str);
}

/**
 * @brief 将字节转换为十六进制数，不包含前置0x
 */
template<>
inline std::string to_string(const unsigned char& value)
{
	std::stringstream ss;
	ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2 * sizeof(unsigned char)) << (unsigned int)value;
	return ss.str();
}

template<>
__attribute__((always_inline)) inline std::string to_string(const bool& value)
{
	return value ? "true" : "false";
}

template<>
__attribute__((always_inline)) inline std::string to_string(const std::string& value)
{
	return value;
}

template<>
__attribute__((always_inline)) inline std::string to_string(const std::stringstream& value)
{
	return value.str();
}

template<typename Clock>
std::string to_string(const std::chrono::time_point<Clock>& value)
{
	time_t time = std::chrono::system_clock::to_time_t(value);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count() % 1000;
	ss << "." << std::setfill('0') << std::setw(3) << ms;
	return ss.str();
}

template<>
inline std::string to_string<void*>(void*const& value)
{
	std::stringstream ss;
	ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2 * sizeof(void*)) << (unsigned long long int)value;
	return ss.str();
}

template<>
__attribute__((always_inline)) inline std::string to_string<char*>(char*const& value)
{
	return std::string(value);
}

template<>
__attribute__((always_inline)) inline std::string to_string<char const*>(char const*const& value)
{
	return std::string(value);
}

/**
 * 字符串拼接
 */
__attribute__((always_inline)) inline void _string_cat(std::stringstream& ss)
{
}

template<typename Arg>
__attribute__((always_inline)) inline void _string_cat(std::stringstream& ss, Arg arg)
{
	ss << to_string(arg);
}

template<typename Arg, typename ...Args>
__attribute__((always_inline)) inline void _string_cat(std::stringstream& ss, Arg arg, Args ...args)
{
	ss << to_string(arg);
	_string_cat(ss, args...);
}

template<typename ...Args>
std::string string_cat(Args ...vals)
{
	std::stringstream ss;
	_string_cat(ss, vals...);
	return ss.str();
}

/**
 * 拷贝字符串
 */
extern const char* str_cpy(const char* str);

extern const char* string_cat(const char** str_arr, int count); //把含有count个字符串的数组合并为一个字符串

template<typename T>
T value_of(const std::string& value);

inline int value_of(const char value)
{
	if(value >= '0' && value <= '9')
		return value - '0';
	else
		return -1;
}

namespace andromeda
{
namespace common
{

class split_strings: public array<const char*>
{
private:
	using array<const char*>::array;
	using array<const char*>::release;
	split_strings() = default;

public:
	~split_strings() = default;

	static split_strings split(const char* str, const char* delim); //编译器将优化，直接在调用处构造返回值。释放返回值中的所有字符串时只需调用free(arr[0])！
};

void pack_bits(unsigned char* bits, int bits_start_pos, unsigned char* bytes, int bytes_start_pos, long int bits_length);

/*
 * @brief 如果是字母则返回大写形式，否则返回本身
 */
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

#endif // ANDROMEDA_COMMON_STRINGUTILS
