#ifndef ANDROMEDA_LOG_LOG
#define ANDROMEDA_LOG_LOG

/**
 * @brief 日志记录类。对于需要引入此头文件的文件，需要将#include <andromeda/log/log.h>放在任何其他引用之前，防止<andromeda/log/signals.h>编译断言失败
 */

#include "../internal/signals.h"
#include "../internal/object.h"
#include <iostream>

#include "../common/string_utils.h"
#include "../common/array.h"

#include "../io/files.h"

namespace andromeda
{
namespace log
{
enum log_level
{
	LOG_DEBUG_INFO = 0, LOG_DEBUG_WARN, LOG_DEBUG_ERROR, LOG_DEBUG_FATAL, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL, LOG_LEVELNUM
};

/**
 * @brief 终端打印日志颜色样式
 */
extern const std::string terminal_fatal_style;
extern const std::string terminal_error_style;
extern const std::string terminal_warn_style;
extern const std::string terminal_reset_style;
extern const std::string terminal_debug_fatal_style;
extern const std::string terminal_debug_error_style;
extern const std::string terminal_debug_warn_style;
extern const std::string terminal_debug_info_style;

template<typename Derived>
class logger
{
private:
	log_level current_log_level = log_level::LOG_INFO;
	bool enable = true;
	andromeda::common::array<object> extra_params;

protected:
	__attribute__((always_inline)) inline static void log_to_terminal(const std::string& text_style, const std::string& info)
	{
		std::cout << andromeda::log::terminal_reset_style << text_style << info << andromeda::log::terminal_reset_style << std::endl;
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
		return object::interpret<T>(extra_params, idx);
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
 * @brief 具有文件记录和终端打印的通用日志记录器
 * 		  额外参数1为日志文件名，额外参数2为终端文本样式字符串
 */
class universal_logger: public logger<universal_logger>
{
protected:
	static constexpr size_t file_path_idx = 0;

	bool log_out_to_terminal = true;
	bool log_out_to_file = true;

	std::string log_text_styles[log_level::LOG_LEVELNUM]{};

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

/**
 * @brief 进程全局日志
 */
extern universal_logger* process_logger;

}
}

/**
 * @brief 为指定日志记录器设置日志等级
 */
#define LogLevelFor(logger_ptr, level)\
		{\
			if(logger_ptr)\
				logger_ptr->set_log_level(level);\
		}

#define LogLevel(level) LogLevelFor(andromeda::log::process_logger, level)

/**
 * @brief 日志打印的源码位置
 * 		  __FILENAME_STRING__为根据__FILE__宏调用函数提取的文件名，不能直接与C字符串拼接
 */
#define __log_source__ __FILENAME_STRING__, ":" __LINE_STRING__ " ", __FUNCTION__, "()"

/**
 * @brief 日志头参数列表
 */
#define __log_header__(...) __macro_with_params__(__log_header__, __VA_ARGS__)

/**
 * @brief 自定义日志头的文件位置，使用时传入参数必须是__log_header__6(level, level_str, __log_source__)
 * 		  __log_source__会在传入前展开，因此参数列表会变为6个
 */
#define __log_header__6(level, level_str, source_file_name, source_line, source_func_name, source_func_paren) level, "[", std::chrono::system_clock::now(), "] [" level_str "] ", source_file_name, source_line, source_func_name, source_func_paren, ": "
#define __log_header__3(level, level_str, log_source) __log_header__6(level, level_str, log_source)
#define __log_header__2(level, level_str) __log_header__6(level, level_str, __log_source__)

/**
 * @brief 将指定的源码位置打印到指定等级的日志，使用时传入参数必须是__log_source_to__(logger_ptr, level, level_str, __log_source__, ...)
 * 		  __log_source__会在传入前展开
 */
#define __log_source_to__(logger_ptr, level, level_str, source_file_name, source_line, source_func_name, source_func_paren, ...)\
		{\
			if(logger_ptr)\
				logger_ptr->log(__log_header__(level, level_str, source_file_name, source_line, source_func_name, source_func_paren), ##__VA_ARGS__);\
		}

#define LogSourceDebugInfoTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_DEBUG_INFO, "DEBUG_INFO", log_source, ##__VA_ARGS__)

#define LogSourceDebugWarnTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_DEBUG_WARN, "DEBUG_WARN", log_source, ##__VA_ARGS__)

#define LogSourceDebugErrorTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_DEBUG_ERROR, "DEBUG_ERROR", log_source, ##__VA_ARGS__)

#define LogSourceDebugFatalTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_DEBUG_FATAL, "DEBUG_FATAL", log_source, ##__VA_ARGS__)

#define LogSourceInfoTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_INFO, "INFO", log_source, ##__VA_ARGS__)

#define LogSourceWarnTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_WARN, "WARN", log_source, ##__VA_ARGS__)

#define LogSourceErrorTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_ERROR, "ERROR", log_source, ##__VA_ARGS__)

#define LogSourceFatalTo(logger_ptr, log_source, ...)\
		__log_source_to__(logger_ptr, andromeda::log::log_level::LOG_FATAL, "FATAL", log_source, ##__VA_ARGS__)

#define LogDebugInfoTo(logger_ptr, ...)\
		LogSourceDebugInfoTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogDebugWarnTo(logger_ptr, ...)\
		LogSourceDebugWarnTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogDebugErrorTo(logger_ptr, ...)\
		LogSourceDebugErrorTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogDebugFatalTo(logger_ptr, ...)\
		LogSourceDebugFatalTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogInfoTo(logger_ptr, ...)\
		LogSourceInfoTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogWarnTo(logger_ptr, ...)\
		LogSourceWarnTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogErrorTo(logger_ptr, ...)\
		LogSourceErrorTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogFatalTo(logger_ptr, ...)\
		LogSourceFatalTo(logger_ptr, __log_source__, ##__VA_ARGS__)

#define LogSourceDebugInfo(log_source, ...)\
		LogSourceDebugInfoTo(andromeda::log::process_logger, log_source, ##__VA_ARGS__)

#define LogSourceDebugWarn(log_source, ...)\
		LogSourceDebugWarnTo(andromeda::log::process_logger, log_source, ##__VA_ARGS__)

#define LogSourceDebugError(log_source, ...)\
		LogSourceDebugErrorTo(andromeda::log::process_logger, log_source,	##__VA_ARGS__)

#define LogSourceDebugFatal(log_source, ...)\
		LogSourceDebugFatalTo(andromeda::log::process_logger, log_source, ##__VA_ARGS__)

#define LogSourceInfo(log_source, ...)\
		LogSourceInfoTo(andromeda::log::process_logger, log_source, ##__VA_ARGS__)

#define LogSourceWarn(log_source, ...)\
		LogSourceWarnTo(andromeda::log::process_logger, log_source, ##__VA_ARGS__)

#define LogSourceError(log_source, ...)\
		LogSourceErrorTo(andromeda::log::process_logger, log_source,	##__VA_ARGS__)

#define LogSourceFatal(log_source, ...)\
		LogSourceFatalTo(andromeda::log::process_logger, log_source, ##__VA_ARGS__)

#define LogDebugInfo(...) LogSourceDebugInfo( __log_source__, ##__VA_ARGS__)

#define LogDebugWarn(...) LogSourceDebugWarn( __log_source__, ##__VA_ARGS__)

#define LogDebugError(...) LogSourceDebugError( __log_source__, ##__VA_ARGS__)

#define LogDebugFatal(...) LogSourceDebugFatal( __log_source__, ##__VA_ARGS__)

#define LogInfo(...) LogSourceInfo( __log_source__, ##__VA_ARGS__)

#define LogWarn(...) LogSourceWarn( __log_source__, ##__VA_ARGS__)

#define LogError(...) LogSourceError( __log_source__, ##__VA_ARGS__)

#define LogFatal(...) LogSourceFatal( __log_source__, ##__VA_ARGS__)

#endif //ANDROMEDA_LOG_LOG
