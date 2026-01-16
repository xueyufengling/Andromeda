#ifndef _CXXCOMM_ANSISTYLE
#define _CXXCOMM_ANSISTYLE

#include <string>

#include <cxxcomm/array.h>

namespace cxxcomm
{

/**
 * @brief 终端字体颜色、样式指令
 */
enum ansi_text_style : int
{
	ANSI_COLOR_256 = 5, //256色提示符
	ANSI_COLOR_RGB = 2, //RGB真彩色提示符

	RESET = 0, //恢复为默认颜色、样式
	BOLD = 1, //加粗
	FAINT = 2, //淡化，与加粗相反
	ITALIC = 3, //斜体
	UNDERLINE = 4, //下划线
	SLOW_BLINK = 5, //缓慢闪烁，少于150次/min
	INVERSE = 7, //反转前景色和背景色
	STRIKE = 9, //删除线
	NOT_BOLD = 21, //取消加粗
	DOUBLELY_UNDERLINED = 21, //双下划线，由于与BOLD_OFF冲突，仅部分系统生效
	NORMAL_INTENSITY = 22, //取消加粗或淡化
	NOT_ITALIC = 23, //取消斜体
	NOT_UNDERLINE = 24, //取消下划线
	NOT_BLINK = 25, //取消闪烁
	NOT_INVERSE = 27, //取消反转前景色和背景色
	REVEAL = 28, //隐藏
	NOT_STRIKE = 29, //取消删除线

	//设置前景色
	SET_FOREGROUND_COLOR_BLACK = 30,
	SET_FOREGROUND_COLOR_RED = 31,
	SET_FOREGROUND_COLOR_GREEN = 32,
	SET_FOREGROUND_COLOR_YELLOW = 33,
	SET_FOREGROUND_COLOR_BLUE = 34,
	SET_FOREGROUND_COLOR_MAGENTA = 35,
	SET_FOREGROUND_COLOR_CYAN = 36,
	SET_FOREGROUND_COLOR_WHITE = 37, //灰白
	SET_FOREGROUND_COLOR_RGB = 38, //自定义前景色，其后跟随的参数为 5;n 或 2;r;g;b
	RESET_FOREGROUND_COLOR = 39, //默认前景色
	SET_FOREGROUND_COLOR_BRIGHT_BLACK = 90, //灰色
	SET_FOREGROUND_COLOR_GRAY = SET_FOREGROUND_COLOR_BRIGHT_BLACK,
	SET_FOREGROUND_COLOR_BRIGHT_RED = 91,
	SET_FOREGROUND_COLOR_BRIGHT_GREEN = 92,
	SET_FOREGROUND_COLOR_BRIGHT_YELLOW = 93,
	SET_FOREGROUND_COLOR_BRIGHT_BLUE = 94,
	SET_FOREGROUND_COLOR_BRIGHT_MAGENTA = 95,
	SET_FOREGROUND_COLOR_BRIGHT_CYAN = 96,
	SET_FOREGROUND_COLOR_BRIGHT_WHITE = 97, //纯白

	//设置背景色
	SET_BACKGROUND_COLOR_BLACK = 40,
	SET_BACKGROUND_COLOR_RED = 41,
	SET_BACKGROUND_COLOR_GREEN = 42,
	SET_BACKGROUND_COLOR_YELLOW = 43,
	SET_BACKGROUND_COLOR_BLUE = 44,
	SET_BACKGROUND_COLOR_MAGENTA = 45,
	SET_BACKGROUND_COLOR_CYAN = 46,
	SET_BACKGROUND_COLOR_WHITE = 47, //灰白
	SET_BACKGROUND_COLOR_RGB = 48, //自定义背景色，其后跟随的参数为 5;n 或 2;r;g;b
	RESET_BACKGROUND_COLOR = 49, //默认背景色
	SET_BACKGROUND_COLOR_BRIGHT_BLACK = 100, //灰色
	SET_BACKGROUND_COLOR_GRAY = SET_BACKGROUND_COLOR_BRIGHT_BLACK,
	SET_BACKGROUND_COLOR_BRIGHT_RED = 101,
	SET_BACKGROUND_COLOR_BRIGHT_GREEN = 102,
	SET_BACKGROUND_COLOR_BRIGHT_YELLOW = 103,
	SET_BACKGROUND_COLOR_BRIGHT_BLUE = 104,
	SET_BACKGROUND_COLOR_BRIGHT_MAGENTA = 105,
	SET_BACKGROUND_COLOR_BRIGHT_CYAN = 106,
	SET_BACKGROUND_COLOR_BRIGHT_WHITE = 107, //纯白

	OVERLINED = 53, //叠印
	NOT_OVERLINED = 55, //取消叠印
};

/**
 * @brief 构造ANSI控制台文本样式字符串
 * @param text_styles 文本样式列表
 * @param text_styles_count 文本样式列表项目个数
 * @param free_text_styles_arr 构造完成后是否释放项目数组
 */
extern std::string pack_ansi_text_style(int* text_styles, size_t text_styles_count, bool free_text_styles_arr = false);

inline std::string pack_ansi_text_style(ansi_text_style* text_styles, size_t text_styles_count, bool free_text_styles_arr = false)
{
	return pack_ansi_text_style((int*)text_styles, text_styles_count, free_text_styles_arr);
}

/**
 * @brief 样式为定长数组，text_styles为左值
 */
template<size_t _StylesNum>
inline std::string pack_ansi_text_style(int (&text_styles)[_StylesNum])
{
	return pack_ansi_text_style(text_styles, _StylesNum, false);
}

/**
 * @brief 样式为定长数组，text_styles为X值
 */
template<size_t _StylesNum>
inline std::string pack_ansi_text_style(int (&&text_styles)[_StylesNum])
{
	return pack_ansi_text_style(text_styles, _StylesNum, false);
}

template<typename ..._Styles>
inline std::string pack_ansi_text_style(_Styles ...styles)
{
	return pack_ansi_text_style(cxxcomm::pack_c_array<int>(styles...), sizeof...(styles), true); //由于pack_c_array构造了数组对象，因此必须在构造样式字符串完成以后回收内存
}

/**
 * @brief 256色的前景色ANSI文本样式
 */
extern std::string foreground_color_256_ansi_text_style(int color);

/**
 * @brief 256色的背景色ANSI文本样式
 */
extern std::string background_color_256_ansi_text_style(int color);

/**
 * @brief 自定义RGB分量的前景色ANSI文本样式
 */
extern std::string foreground_color_rgb_ansi_text_style(int r, int g, int b);

/**
 * @brief 自定义RGB分量的背景色ANSI文本样式
 */
extern std::string background_color_rgb_ansi_text_style(int r, int g, int b);

}

#endif //_CXXCOMM_ANSISTYLE
