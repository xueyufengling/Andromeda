#include <andromeda/common/terminal_style.h>

#include <sstream>

std::string andromeda::common::terminal_style(int* text_styles, size_t text_styles_count, bool free_text_styles_arr)
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
		ss << (int)terminal_text_style::RESET; //如果没有传入任何样式或传入空指针，那么将样式设置为0
	ss << "m";
	if(free_text_styles_arr)
		delete[] text_styles;
	return ss.str();
}
