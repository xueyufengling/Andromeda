#ifndef ANDROMEDA_COMMON_TERMINALSTYLE
#define ANDROMEDA_COMMON_TERMINALSTYLE

#include <string>

namespace andromeda
{
namespace common
{
/**
 * @brief 终端前景色，即字体颜色
 */
enum class terminal_foreground_color : int
{
	RESET = 0, //默认前景色
	BLACK = 30,
	RED = 31,
	GREEN = 32,
	YELLOW = 33,
	BLUE = 34,
	MAGENTA = 35,
	CYAN = 36,
	WHITE = 37,
};

/**
 * @brief 终端背景色
 */
enum class terminal_background_color : int
{
	RESET = 0, //默认背景色
	BLACK = 40,
	RED = 41,
	GREEN = 42,
	YELLOW = 43,
	BLUE = 44,
	MAGENTA = 45,
	CYAN = 46,
	WHITE = 47,
};

/**
 * @brief 终端字体样式
 */
enum class terminal_text_style : int
{
	RESET = 0, //恢复为默认样式
	BOLD = 1, //加粗
	UNDERLINE = 4, //下划线
	INVERSE = 7, //反转前景色和背景色
	BOLD_OFF = 21, //取消加粗
	UNDERLINE_OFF = 24, //取消下划线
	INVERSE_OFF = 27, //取消反转前景色和背景色
};

extern std::string terminal_style(terminal_foreground_color foreground_color, terminal_background_color background_color, terminal_text_style* text_styles, size_t text_styles_count);

/**
 * @brief 样式为定长数组，text_styles为左值
 */
template<size_t StylesNum>
inline std::string terminal_style(terminal_foreground_color foreground_color, terminal_background_color background_color, terminal_text_style (&text_styles)[StylesNum])
{
	return terminal_style(foreground_color, background_color, text_styles, StylesNum);
}

/**
 * @brief 样式为定长数组，text_styles为X值
 */
template<size_t StylesNum>
inline std::string terminal_style(terminal_foreground_color foreground_color, terminal_background_color background_color, terminal_text_style (&&text_styles)[StylesNum])
{
	return terminal_style(foreground_color, background_color, text_styles, StylesNum);
}

inline std::string terminal_style(terminal_foreground_color foreground_color = terminal_foreground_color::RESET, terminal_background_color background_color = terminal_background_color::RESET, terminal_text_style text_style = terminal_text_style::RESET)
{
	return terminal_style(foreground_color, background_color, &text_style, 1);
}

extern const std::string terminal_reset_style;
extern const std::string terminal_fatal_style;
extern const std::string terminal_error_style;
extern const std::string terminal_warn_style;
extern const std::string terminal_debug_style;

}
}

#endif //ANDROMEDA_COMMON_TERMINALSTYLE
