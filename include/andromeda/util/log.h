#ifndef ANDROMEDA_UTIL_LOG
#define ANDROMEDA_UTIL_LOG

#include <iostream>
#include "string_utils.h"

#define LOGGER_TYPE terminal_logger

namespace andromeda
{
namespace util
{
class LOGGER_TYPE;

enum log_level
{
	LOG_DEBUG = 0, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL
};

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
			((Derived*)this)->log_out(string_cat(info...));
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

class terminal_logger: public logger<terminal_logger>
{
public:
	__attribute__((always_inline)) void log_out(std::string info)
	{
		std::cout << info << std::endl;
	}
};

}
}

#define LogDebugTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::util::log_level::LOG_DEBUG, "[", std::chrono::system_clock::now(), "] [DEBUG] " __FILE__ ":" __LINE_STRING__ " " __FUNCTION_STRING__ ": ", ##__VA_ARGS__);\
		}

#define LogInfoTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::util::log_level::LOG_INFO, "[", std::chrono::system_clock::now(), "] [INFO] " __FILE__ ":" __LINE_STRING__ " " __FUNCTION_STRING__ ": ", ##__VA_ARGS__);\
		}

#define LogWarnTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::util::log_level::LOG_WARN, "[", std::chrono::system_clock::now(), "] [WARN] " __FILE__ ":" __LINE_STRING__ " " __FUNCTION_STRING__ ": ", ##__VA_ARGS__);\
		}

#define LogErrorTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::util::log_level::LOG_ERROR, "[", std::chrono::system_clock::now(), "] [ERROR] " __FILE__ ":" __LINE_STRING__ " " __FUNCTION_STRING__ ": ", ##__VA_ARGS__);\
		}

#define LogFatalTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::util::log_level::LOG_FATAL, "[", std::chrono::system_clock::now(), "] [FATAL] " __FILE__ ":" __LINE_STRING__ " " __FUNCTION_STRING__ ": ", ##__VA_ARGS__);\
		}

#define LogDebug(...) LogDebugTo(andromeda::util::process_logger, ##__VA_ARGS__)

#define LogInfo(...) LogInfoTo(andromeda::util::process_logger, ##__VA_ARGS__)

#define LogWarn(...) LogWarnTo(andromeda::util::process_logger, ##__VA_ARGS__)

#define LogError(...) LogErrorTo(andromeda::util::process_logger, ##__VA_ARGS__)

#define LogFatal(...) LogFatalTo(andromeda::util::process_logger, ##__VA_ARGS__)

#endif //ANDROMEDA_UTIL_LOG
