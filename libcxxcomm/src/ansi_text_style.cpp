#include <cxxcomm/ansi_text_style.h>
#include <sstream>

std::string cxxcomm::pack_ansi_text_style(int* text_styles, size_t text_styles_count, bool free_text_styles_arr)
{
	std::stringstream ss;
	ss << "\033[";
	if(text_styles && text_styles_count > 0)
	{
		for(size_t idx = 0; idx < text_styles_count; ++idx)
		{
			ss << *(text_styles + idx);
			if(idx < text_styles_count - 1)
				ss << ";";
		}
	}
	else
		ss << (int)ansi_text_style::RESET; //如果没有传入任何样式或传入空指针，那么将样式设置为0
	ss << "m";
	if(free_text_styles_arr)
		delete[] text_styles;
	return ss.str();
}

std::string cxxcomm::foreground_color_256_ansi_text_style(int color)
{
	return pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_RGB, ansi_text_style::ANSI_COLOR_256, color);
}

std::string cxxcomm::background_color_256_ansi_text_style(int color)
{
	return pack_ansi_text_style(ansi_text_style::SET_BACKGROUND_COLOR_RGB, ansi_text_style::ANSI_COLOR_256, color);
}

std::string cxxcomm::foreground_color_rgb_ansi_text_style(int r, int g, int b)
{
	return pack_ansi_text_style(ansi_text_style::SET_FOREGROUND_COLOR_RGB, ansi_text_style::ANSI_COLOR_RGB, r, g, b);
}

std::string cxxcomm::background_color_rgb_ansi_text_style(int r, int g, int b)
{
	return pack_ansi_text_style(ansi_text_style::SET_BACKGROUND_COLOR_RGB, ansi_text_style::ANSI_COLOR_RGB, r, g, b);
}
