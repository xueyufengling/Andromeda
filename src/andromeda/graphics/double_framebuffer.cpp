#include <andromeda/graphics/double_framebuffer.h>

using namespace andromeda::graphics;

void double_framebuffer::alloc(bool try_again)
{
	back_frame_buffer.alloc(try_again);
}

