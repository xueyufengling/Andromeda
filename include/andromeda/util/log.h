#ifndef ANDROMEDA_UTIL_LOG
#define ANDROMEDA_UTIL_LOG
//定义调试相关的宏，例如输出报错信息的方式

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#define LOGGER_TYPE terminal_logger

template<typename T>
std::string to_string(T& value);

#define StdToString(type)\
template<>\
std::string to_string(type& value)\
{\
	return std::to_string(value);\
}

StdToString(double)
StdToString(float)
StdToString(int)
StdToString(long double)
StdToString(long int)
StdToString(long long int)
StdToString(unsigned int)
StdToString(unsigned long int)
StdToString(unsigned long long int)

StdToString(short)
StdToString(unsigned short)

#undef StdToString

template<>
std::string to_string(char& value)
{
	return std::string(value);
}

template<>
std::string to_string(unsigned char& value)
{
	std::stringstream ss;
	ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2 * sizeof(unsigned char)) << (int)value;
	return ss.str();
}

template<>
std::string to_string(void*& value)
{
	std::stringstream ss;
	ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2 * sizeof(void*)) << (unsigned long long int)value;
	return ss.str();
}

template<>
std::string to_string(std::string& value)
{
	return value;
}

template<>
std::string to_string(const char*& value)
{
	return std::string(value);
}

namespace andromeda
{
namespace util
{
class LOGGER_TYPE;

enum log_level
{
	LOG_DEBUG = 0, LOG_INFO, LOG_ERROR
};

template<typename Arg, typename ...Args>
__attribute__((always_inline)) inline void _string_cat(std::stringstream& ss, Arg arg, Args ...args)
{
	ss << to_string<Arg>(arg);
	_string_cat(ss, args...);
}

template<>
__attribute__((always_inline)) inline void _string_cat(std::stringstream& ss)
{
}

template<typename ...Args>
std::string string_cat(Args ...vals)
{
	std::stringstream ss;
	_info_to_string(ss, vals...);
	return ss.str();
}

template<typename Derived>
class logger
{
private:
	log_level current_log_level = log_level::LOG_INFO;

public:
	void set_log_level(log_level new_log_level)
	{
		current_log_level = new_log_level;
	}

	template<typename ...Args>
	__attribute__((always_inline)) inline void log(log_level msg_level, Args ...info)
	{
		if(current_log_level <= msg_level)
			(Derived*)this->log_out(string_cat(info...));
	}

	template<typename ...Args>
	__attribute__((always_inline)) inline void log(log_level msg_level)
	{
	}
};

/**
 * @brief 进程全局日志
 */
extern LOGGER_TYPE* process_logger;

class terminal_logger : public logger<terminal_logger>
{
public:
	__attribute__((always_inline)) void log_out(std::string& info)
	{
		std::cout << info << std::endl;
	}
};

}
}

#define LogDebug(...) {andromeda::util::process_logger->log(andromeda::util::log_level::LOG_DEBUG, ##__VA_ARGS__);}

#define LogInfo(...) {andromeda::util::process_logger->log(andromeda::util::log_level::LOG_INFO, ##__VA_ARGS__);}

#define LogError(...) {andromeda::util::process_logger->log(andromeda::util::log_level::LOG_ERROR, ##__VA_ARGS__);}

#endif //ANDROMEDA_UTIL_LOG
