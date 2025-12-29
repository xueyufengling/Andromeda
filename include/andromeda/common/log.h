#ifndef ANDROMEDA_COMMON_LOG
#define ANDROMEDA_COMMON_LOG

#include <iostream>

#include "string_utils.h"
#include "object_val.h"
#include "array.h"

#include "../io/files.h"

#define LOGGER_TYPE universal_logger

namespace andromeda
{
namespace common
{
class LOGGER_TYPE;

enum log_level
{
	LOG_DEBUG = 0, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL, LOG_LEVELNUM
};

/**
 * @brief 终端打印日志颜色样式
 */
extern const std::string terminal_fatal_style;
extern const std::string terminal_error_style;
extern const std::string terminal_warn_style;
extern const std::string terminal_reset_style;
extern const std::string terminal_debug_style;

template<typename Derived>
class logger
{
private:
	log_level current_log_level = log_level::LOG_INFO;
	bool enable = true;
	array<object_val> extra_params;

protected:
	__attribute__((always_inline)) inline static void log_to_terminal(const std::string& text_style, const std::string& info)
	{
		std::cout << andromeda::common::terminal_reset_style << text_style << info << andromeda::common::terminal_reset_style << std::endl;
	}

	__attribute__((always_inline)) inline static void log_to_file(const std::string& file_path, const std::string& info)
	{
		andromeda::io::append_string_newline(file_path, info);
	}

	template<typename T>
	void add_extra_param(const T& extra_param)
	{
		this->extra_params.add(extra_param);
	}

	template<typename T>
	void set_extra_param(int idx, const T& extra_param)
	{
		this->extra_params[idx] = extra_param;
	}

	template<typename T>
	T& get_extra_param(int idx)
	{
		return object_val::fetch<T>(extra_params, idx);
	}

public:
	inline void set_enable(bool enable)
	{
		this->enable = enable;
	}

	inline void is_enable(bool enable)
	{
		this->enable = enable;
	}

	void set_log_level(log_level new_log_level)
	{
		current_log_level = new_log_level;
	}

	template<typename ...Args>
	__attribute__((always_inline)) inline void log(log_level msg_level, Args ...info)
	{
		if(enable && current_log_level <= msg_level)
			((Derived*)this)->log_out(msg_level, string_cat(info...));
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

/**
 * @brief 具有文件记录和终端打印的通用日志记录器
 * 		  额外参数1为日志文件名，额外参数2为终端文本样式字符串
 */
class universal_logger: public logger<universal_logger>
{
protected:
	static constexpr size_t file_path_idx = 0;

	bool log_out_to_terminal = true;
	bool log_out_to_file = true;

	array<std::string> log_text_styles;

public:
	universal_logger();

	/**
	 * @brief 设置是否开启终端打印输出
	 */
	__attribute__((always_inline)) inline void set_log_to_terminal(bool log_out_to_terminal)
	{
		this->log_out_to_terminal = log_out_to_terminal;
	}

	/**
	 * @brief 设置是否开启文件输出
	 */
	__attribute__((always_inline)) inline void set_log_to_file(bool log_out_to_file)
	{
		this->log_out_to_file = log_out_to_file;
	}

	__attribute__((always_inline)) inline void set_text_style(log_level msg_level, const std::string& text_style)
	{
		log_text_styles[msg_level] = text_style;
	}

	void set_default_text_styles();

	/**
	 * @brief 获取不同等级的终端文本样式
	 */
	__attribute__((always_inline)) inline std::string& get_text_style(log_level msg_level)
	{
		return log_text_styles[msg_level];
	}

	/**
	 * @brief 获取当前日志文件名
	 */
	__attribute__((always_inline)) inline const std::string& log_file_path()
	{
		return get_extra_param<std::string>(file_path_idx);
	}

	void log_out(log_level msg_level, const std::string info);
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

/**
 * @brief 为指定日志记录器设置日志等级
 */
#define LogLevelFor(logger_ptr, level)\
		{\
			if(logger_ptr)\
				logger_ptr->set_log_level(level);\
		}

#define LogDebug(...) LogDebugTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogInfo(...) LogInfoTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogWarn(...) LogWarnTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogError(...) LogErrorTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogFatal(...) LogFatalTo(andromeda::common::process_logger, ##__VA_ARGS__)

#define LogLevel(level) LogLevelFor(andromeda::common::process_logger, level)

#endif //ANDROMEDA_COMMON_LOG
