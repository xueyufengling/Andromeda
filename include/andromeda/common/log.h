#ifndef ANDROMEDA_COMMON_LOG
#define ANDROMEDA_COMMON_LOG

#include <andromeda/common/string_utils.h>
#include <iostream>

#define LOGGER_TYPE terminal_logger

namespace andromeda
{
namespace common
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

/**
 * __FUNCTION__不是宏，而是函数内变量，需要单独作为一个值输出，不能直接与C字符串拼接
 * __FILENAME_STRING__为根据__FILE__宏调用函数提取的文件名，不能直接与C字符串拼接
 */
#define LogDebugTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::common::log_level::LOG_DEBUG, "[", std::chrono::system_clock::now(), "] [DEBUG] ", __FILENAME_STRING__, ":" __LINE_STRING__ " ", __FUNCTION__, "(): ", ##__VA_ARGS__);\
		}

#define LogInfoTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::common::log_level::LOG_INFO, "[", std::chrono::system_clock::now(), "] [INFO] ", __FILENAME_STRING__, ":" __LINE_STRING__ " ", __FUNCTION__, "(): ", ##__VA_ARGS__);\
		}

#define LogWarnTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::common::log_level::LOG_WARN, "[", std::chrono::system_clock::now(), "] [WARN] ", __FILENAME_STRING__, ":" __LINE_STRING__ " ", __FUNCTION__, "(): ", ##__VA_ARGS__);\
		}

#define LogErrorTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::common::log_level::LOG_ERROR, "[", std::chrono::system_clock::now(), "] [ERROR] ", __FILENAME_STRING__, ":" __LINE_STRING__ " ", __FUNCTION__, "(): ", ##__VA_ARGS__);\
		}

#define LogFatalTo(logger_ptr, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(andromeda::common::log_level::LOG_FATAL, "[", std::chrono::system_clock::now(), "] [FATAL] ", __FILENAME_STRING__, ":" __LINE_STRING__ " ", __FUNCTION__, "(): ", ##__VA_ARGS__);\
		}

#define LogDebug(...) LogDebugTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogInfo(...) LogInfoTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogWarn(...) LogWarnTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogError(...) LogErrorTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogFatal(...) LogFatalTo(andromeda::common::process_logger, ##__VA_ARGS__)

#endif //ANDROMEDA_COMMON_LOG
