#include <cxxcomm/log.h>

#include <cxxcomm/ansi_text_style.h>

using namespace cxxcomm;

const std::string cxxcomm::terminal_fatal_style = cxxcomm::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_BRIGHT_WHITE, ansi_text_style::SET_BACKGROUND_COLOR_BRIGHT_RED);
const std::string cxxcomm::terminal_error_style = cxxcomm::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_RED);
const std::string cxxcomm::terminal_warn_style = cxxcomm::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_YELLOW);
const std::string cxxcomm::terminal_reset_style = cxxcomm::pack_ansi_text_style(ansi_text_style::RESET);
const std::string cxxcomm::terminal_debug_fatal_style = cxxcomm::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_BRIGHT_WHITE, ansi_text_style::SET_BACKGROUND_COLOR_BRIGHT_MAGENTA);
const std::string cxxcomm::terminal_debug_error_style = cxxcomm::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_MAGENTA);
const std::string cxxcomm::terminal_debug_warn_style = cxxcomm::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_CYAN);
const std::string cxxcomm::terminal_debug_info_style = cxxcomm::pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_GREEN);

universal_logger* cxxcomm::process_logger = new universal_logger();

universal_logger::universal_logger()
{
	//add_extra_param("D:\\log\\log_file.log");
	set_default_text_styles();

}

void universal_logger::set_default_text_styles()
{
	set_text_style(log_level::LOG_FATAL, cxxcomm::terminal_fatal_style);
	set_text_style(log_level::LOG_ERROR, cxxcomm::terminal_error_style);
	set_text_style(log_level::LOG_WARN, cxxcomm::terminal_warn_style);
	set_text_style(log_level::LOG_INFO, cxxcomm::terminal_reset_style);
	set_text_style(log_level::LOG_DEBUG_FATAL, cxxcomm::terminal_debug_fatal_style);
	set_text_style(log_level::LOG_DEBUG_ERROR, cxxcomm::terminal_debug_error_style);
	set_text_style(log_level::LOG_DEBUG_WARN, cxxcomm::terminal_debug_warn_style);
	set_text_style(log_level::LOG_DEBUG_INFO, cxxcomm::terminal_debug_info_style);
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
