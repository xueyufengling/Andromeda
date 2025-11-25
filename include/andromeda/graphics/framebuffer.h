#ifndef ANDROMEDA_GRAPHICS_FRAMEBUFFER
#define ANDROMEDA_GRAPHICS_FRAMEBUFFER

#include "gl_basic.h"
#include "../graphics/color_rgba.h"
#include "shader_program.h"

//BUFFER_SIZE宏用于得到分配内存使用的实际尺寸，在Mac OS上尺寸要乘以2
#ifdef __APPLE__
#define FRAMEBUFFER_BUFFER_SIZE(size) (size*2)
#else
#define FRAMEBUFFER_BUFFER_SIZE(size) (size)
#endif

namespace andromeda
{
namespace graphics
{
class framebuffer : public andromeda::graphics::gl_object<framebuffer>
{
private:
	GLuint color_buffer = 0;
	GLuint depth_stencil_buffer = 0;
	int width, height;
	bool allocated = false;
	andromeda::graphics::color_rgba clear_color;
	GLuint frame_vao = 0; //OpenGL的VAO对象，储存了顶点属性、VBO、EBO数据缓冲id
	GLuint frame_vbo = 0; //用于渲染到屏幕使用
	GLuint frame_ebo = 0;

public:
	//依次序为左下角、左上角、右下角、右上角，包含位置（NDC坐标）、顶点颜色、纹理坐标
	static float full_screen_ndc_vertices_pt_data[];
	static unsigned int full_screen_ndc_vertices_elements[4];

	/**
	 * 构造函数不会实际初始化和分配缓冲id，这将在alloc()中执行
	 */
	framebuffer() = default;

	/*
	 * 构造函数疑难：下面的构造函数在头文件中定义可以正常工作，但在.cpp文件中定义则会访问空指针异常
	 */
	framebuffer(int width, int height, andromeda::graphics::color_rgba clear_color = {0, 0, 0, 0}) :
			width(width), height(height), clear_color(clear_color)
	{
		set_clear_color(clear_color);
	}

	inline void set_clear_color(andromeda::graphics::color_rgba clear_color = {0, 0, 0, 0})
	{
		this->clear_color = clear_color;
	}

	inline andromeda::graphics::color_rgba get_clear_color()
	{
		return clear_color;
	}

	inline int get_framebuffer_width()
	{
		return width;
	}

	inline int get_framebuffer_height()
	{
		return height;
	}

	__attribute__((always_inline)) inline static void bind(GLuint framebuffer_id)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	}

	/**
	 * OpenGL的查询代价高昂，应当尽量避免查询
	 */
	__attribute__((always_inline)) inline static GLuint current_bind()
	{
		return gl_get_integer(GL_FRAMEBUFFER_BINDING);
	}

	/**
	 * @brief 销毁帧缓冲及其附属的纹理、深度模板缓冲
	 */
	static void invalidate(GLuint framebuffer_id);

	static GLuint get_framebuffer_texture(GLuint dest_frame_buffer, int texture_attachment = 0); //传入颜色缓冲序号，如果传入dest_frame_buffer=0（默认帧缓冲）则依照texture_attachment次序由0-3返回GL_FRONT_LEFT,GL_BACK_LEFT,GL_FRONT_RIGHT,GL_BACK_RIGHT
	static GLuint get_framebuffer_depth_stencil(GLuint dest_frame_buffer); //传入渲染缓冲序号

	inline GLuint get_framebuffer_texture(int texture_attachment = 0)
	{
		return get_framebuffer_texture(id(), texture_attachment);
	}

	inline GLuint get_framebuffer_depth_stencil_buffer()
	{
		return get_framebuffer_depth_stencil(id());
	}

	__attribute__((always_inline)) inline void clear_all_buffers() //清除所有缓存数据
	{
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	__attribute__((always_inline)) inline void clear_color_buffer() //只清除颜色缓冲
	{
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	__attribute__((always_inline)) static inline void clear_depth_stencil_buffer() //只清除颜色缓冲
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	bool alloc(int try_timeout = 1); //分配内存，需要在使用前最先调用。try_timeout为尝试分配的次数，返回值为分配是否成功

	void copy_color_buffer(GLuint dest_frame_buffer, int color_buffer_attachment = 0); //将本帧缓冲拷贝到另一个帧缓存上。dest_frame_buffer=0则拷贝进屏幕缓冲，此时color_buffer_attachment参数无用

	/*
	 * @brief 渲染完成后调用该函数将数据渲染到屏幕缓存部分区域(采用NDC坐标)，渲染后frame_buffer绑定到0（屏幕缓冲）
	 * @param vertex_arr 需要指定四个点（依次序为左下角、左上角、右下角、右上角，包含位置、颜色、纹理坐标）用于渲染到屏幕缓冲，通常不需要调用这个函数
	 */
	void render_to_screen(float* vertex_arr);

	/**
	 * @brief 完全填充屏幕缓存，将该帧缓冲的颜色缓冲渲染到屏幕上。内部使用render_to_screen()实现
	 */
	__attribute__((always_inline)) inline void blit_to_screen()
	{
		render_to_screen(full_screen_ndc_vertices_pt_data);
	}
}
;

}
}

#endif //ANDROMEDA_GRAPHICS_FRAMEBUFFER
