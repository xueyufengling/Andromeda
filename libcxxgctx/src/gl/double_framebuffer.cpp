#include <cxxgctx/gl/double_framebuffer.h>

using namespace cxxgctx::gl;

void double_framebuffer::alloc(bool try_again)
{
	back_frame_buffer.alloc(try_again);
}

