#include <andromeda/common/terminal_style.h>

#include <sstream>

const std::string andromeda::common::terminal_reset_style = andromeda::common::terminal_style(terminal_foreground_color::RESET, terminal_background_color::RESET);
const std::string andromeda::common::terminal_fatal_style = andromeda::common::terminal_style(terminal_foreground_color::WHITE, terminal_background_color::RED);
const std::string andromeda::common::terminal_error_style = andromeda::common::terminal_style(terminal_foreground_color::RED, terminal_background_color::RESET);
const std::string andromeda::common::terminal_warn_style = andromeda::common::terminal_style(terminal_foreground_color::YELLOW, terminal_background_color::RESET);
const std::string andromeda::common::terminal_debug_style = andromeda::common::terminal_style(terminal_foreground_color::GREEN, terminal_background_color::RESET);

std::string andromeda::common::terminal_style(terminal_foreground_color foreground_color, terminal_background_color background_color, terminal_text_style* text_styles, size_t text_styles_count)
{
	std::stringstream ss;
	ss << "\033[";
	if(foreground_color != terminal_foreground_color::RESET)
	{
		ss << (int)foreground_color;
	}
	if(background_color != terminal_background_color::RESET)
	{
		if(foreground_color != terminal_foreground_color::RESET)
			ss << ";";
		ss << (int)background_color;
	}
	bool has_valid_sign = foreground_color != terminal_foreground_color::RESET || background_color != terminal_background_color::RESET; //当前景色、后景色、样式存在非零值时，该值始终为true
	if(text_styles_count > 0)
	{
		for(size_t idx = 0; idx < text_styles_count; ++idx)
		{
			terminal_text_style text_style = *(text_styles + idx);
			if(text_style == terminal_text_style::RESET) //本次样式是RESET，则直接忽略
				continue;
			else
				has_valid_sign = true;
			if(has_valid_sign)
				ss << ";";
			ss << (int)text_style;
		}
	}
	if(!has_valid_sign) //如果所有前景色、背景色、样式都是RESET标识，那么将样式设置为0
		ss << (int)terminal_text_style::RESET;
	ss << "m";
	return ss.str();
}
