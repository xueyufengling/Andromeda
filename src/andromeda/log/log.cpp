#include <andromeda/log/log.h>

#include <andromeda/common/ansi_text_style.h>

using namespace andromeda::log;
using namespace andromeda::common;

const std::string andromeda::log::terminal_fatal_style = andromeda::common::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_BRIGHT_WHITE, ansi_text_style::SET_BACKGROUND_COLOR_BRIGHT_RED);
const std::string andromeda::log::terminal_error_style = andromeda::common::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_RED);
const std::string andromeda::log::terminal_warn_style = andromeda::common::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_YELLOW);
const std::string andromeda::log::terminal_reset_style = andromeda::common::pack_ansi_text_style(ansi_text_style::RESET);
const std::string andromeda::log::terminal_debug_fatal_style = andromeda::common::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_BRIGHT_WHITE, ansi_text_style::SET_BACKGROUND_COLOR_BRIGHT_MAGENTA);
const std::string andromeda::log::terminal_debug_error_style = andromeda::common::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_MAGENTA);
const std::string andromeda::log::terminal_debug_warn_style = andromeda::common::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_CYAN);
const std::string andromeda::log::terminal_debug_info_style = andromeda::common::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_GREEN);

universal_logger* andromeda::log::process_logger = new universal_logger();

universal_logger::universal_logger()
{
	//add_extra_param("D:\\log\\log_file.log");
	set_default_text_styles();

}

void universal_logger::set_default_text_styles()
{
	set_text_style(log_level::LOG_FATAL, andromeda::log::terminal_fatal_style);
	set_text_style(log_level::LOG_ERROR, andromeda::log::terminal_error_style);
	set_text_style(log_level::LOG_WARN, andromeda::log::terminal_warn_style);
	set_text_style(log_level::LOG_INFO, andromeda::log::terminal_reset_style);
	set_text_style(log_level::LOG_DEBUG_FATAL, andromeda::log::terminal_debug_fatal_style);
	set_text_style(log_level::LOG_DEBUG_ERROR, andromeda::log::terminal_debug_error_style);
	set_text_style(log_level::LOG_DEBUG_WARN, andromeda::log::terminal_debug_warn_style);
	set_text_style(log_level::LOG_DEBUG_INFO, andromeda::log::terminal_debug_info_style);
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
