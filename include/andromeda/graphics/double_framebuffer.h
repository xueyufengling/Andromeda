#ifndef ANDROMEDA_GRAPHICS_DOUBLEFRAMEBUFFER
#define ANDROMEDA_GRAPHICS_DOUBLEFRAMEBUFFER

#include "../graphics/color_rgba.h"
#include "framebuffer.h"

namespace andromeda
{
namespace graphics
{
//初始化对象后，调用alloc()分配内存，每次渲染循环开始调用use()，渲染循环结束调用swap()即可
class double_framebuffer
{
private:
	framebuffer back_frame_buffer;
	public:
	double_framebuffer() = default;

	double_framebuffer(int width, int height, andromeda::graphics::color_rgba clearColor = {0, 0, 0, 0}) :
			back_frame_buffer(width, height, clearColor)
	{
	}

	void alloc(bool try_again = true); //分配内存

	operator framebuffer*()
	{
		return &back_frame_buffer;
	}

	operator framebuffer&()
	{
		return back_frame_buffer;
	}

	inline void set_clear_color(andromeda::graphics::color_rgba clear_color = {0, 0, 0, 0})
	{
		back_frame_buffer.set_clear_color(clear_color);
	}

	inline int get_framebuffer_width()
	{
		return back_frame_buffer.get_framebuffer_width();
	}

	inline int get_framebuffer_height()
	{
		return back_frame_buffer.get_framebuffer_height();
	}

	__attribute__((always_inline)) inline void use()
	{
		back_frame_buffer.bind_this();
		back_frame_buffer.clear_color_buffer(); //清除当前帧的内容
	}

	__attribute__((always_inline)) inline void swap()
	{
		back_frame_buffer.blit_to_screen(); //当前帧渲染到屏幕
		gl_Flush();
	}
};
}
}

#endif//ANDROMEDA_GRAPHICS_DOUBLEFRAMEBUFFER
