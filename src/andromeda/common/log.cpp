#include <andromeda/common/log.h>

using namespace andromeda::common;

LOGGER_TYPE* andromeda::common::process_logger = new LOGGER_TYPE();

universal_logger::universal_logger()
{
	add_extra_param("D:\\log\\log_file.log");
	set_default_text_styles();
}

void universal_logger::set_default_text_styles()
{
	set_text_style(log_level::LOG_FATAL, andromeda::common::terminal_fatal_style);
	set_text_style(log_level::LOG_ERROR, andromeda::common::terminal_error_style);
	set_text_style(log_level::LOG_WARN, andromeda::common::terminal_warn_style);
	set_text_style(log_level::LOG_INFO, andromeda::common::terminal_reset_style);
	set_text_style(log_level::LOG_DEBUG, andromeda::common::terminal_debug_style);
}

void universal_logger::log_out(log_level msg_level, const std::string info)
{
	if(log_out_to_terminal)
	{
		logger<universal_logger>::log_to_terminal(get_text_style(msg_level), info);
	}
	if(log_out_to_file)
	{
		//logger<universal_logger>::log_to_file(log_file_path(), info);
	}
}
