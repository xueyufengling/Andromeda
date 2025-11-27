#include <andromeda/common/log.h>
#include <andromeda/graphics/framebuffer.h>

#include <andromeda/graphics/shader_program.h>
#include <andromeda/graphics/internal_shaders.h>
#include <andromeda/graphics/vertex_attribute.h>

using namespace andromeda::graphics;

bool framebuffer::alloc(int try_timeout)
{
	ALLOC: if(allocated)
		clear_all_buffers();
	else
	{
		gl_GenFramebuffers(1, &obj_id); //生成帧缓冲
		gl_BindFramebuffer(GL_FRAMEBUFFER, obj_id);
		gl_GenTextures(1, &color_buffer); //生成颜色缓冲，可读可写
		gl_BindTexture(GL_TEXTURE_2D, color_buffer);
		gl_TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FRAMEBUFFER_BUFFER_SIZE(width), FRAMEBUFFER_BUFFER_SIZE(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //为颜色缓冲分配内存
		gl_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //设置颜色缓冲采样格式
		gl_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gl_FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0); //将颜色缓冲绑定到帧缓冲obj_id
		gl_GenRenderbuffers(1, &depth_stencil_buffer); //生成渲染缓冲的深度、模板缓冲部件，GL_RENDERBUFFER效率比GL_TEXTURE_2D更高，该缓冲是只写的
		gl_BindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);
		gl_RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FRAMEBUFFER_BUFFER_SIZE(width), FRAMEBUFFER_BUFFER_SIZE(height)); //为深度模板缓冲分配内存
		gl_FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_buffer); //将深度模板缓冲绑定到obj_id
		gl_BindFramebuffer(GL_FRAMEBUFFER, 0); //解绑帧缓冲
		gl_BindTexture(GL_TEXTURE_2D, 0);
		gl_BindRenderbuffer(GL_RENDERBUFFER, 0);
		gl_GenBuffers(1, &frame_vbo);
		gl_GenBuffers(1, &frame_ebo);
		gl_GenVertexArrays(1, &frame_vao);
		--try_timeout;
	}
	if(gl_CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //如果分配失败或者不完整
	{
		if(try_timeout < 0) //用尽了尝试分配的次数则返回分配失败
		{
			LogDebug("Framebuffer initialize failed.")
			return false;
		}
		else
			goto ALLOC;
	}
	allocated = true;
	return true;
}

void framebuffer::invalidate(GLuint framebuffer_id)
{
	gl_DeleteFramebuffers(1, &framebuffer_id);
	GLuint color_buffer = get_framebuffer_texture(framebuffer_id, 0);
	gl_DeleteTextures(1, &color_buffer);
	GLuint depth_stencil_buffer = get_framebuffer_depth_stencil(framebuffer_id);
	gl_DeleteRenderbuffers(1, &depth_stencil_buffer);
}

GLuint framebuffer::get_framebuffer_texture(GLuint dest_framebuffer_id, int texture_attachment) //传入颜色缓冲序号
{
	GLint dest_texture;
	gl_BindFramebuffer(GL_FRAMEBUFFER, dest_framebuffer_id);
	if(dest_framebuffer_id) //用户创建缓冲
		gl_GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &dest_texture);
	else
		//默认缓冲
		gl_GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_FRONT_LEFT + texture_attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &dest_texture);
	gl_BindFramebuffer(GL_FRAMEBUFFER, 0);
	return dest_texture;
}

GLuint framebuffer::get_framebuffer_depth_stencil(GLuint dest_framebuffer_id) //传入颜色缓冲序号
{
	GLint dest_depth_stencil;
	gl_BindFramebuffer(GL_FRAMEBUFFER, dest_framebuffer_id);
	gl_GetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &dest_depth_stencil);
	gl_BindFramebuffer(GL_FRAMEBUFFER, 0);
	return dest_depth_stencil;
}

void framebuffer::copy_color_buffer(GLuint dest_framebuffer_id, int color_buffer_attachment)
{
	if(dest_framebuffer_id)
	{
		GLuint dest_texture = get_framebuffer_texture(dest_framebuffer_id, color_buffer_attachment);
		gl_BindFramebuffer(GL_FRAMEBUFFER, dest_framebuffer_id);
		gl_BindTexture(GL_TEXTURE_2D, dest_texture);
		gl_CopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
		gl_BindTexture(GL_TEXTURE_2D, 0);
		gl_BindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
		blit_to_screen();
}

void framebuffer::render_to_screen(float* vertex_arr)
{
	gl_BindFramebuffer(GL_FRAMEBUFFER, 0); //绑定到屏幕帧缓冲
	gl_Disable(GL_DEPTH_TEST);
	clear_color_buffer();
	andromeda::graphics::pt_default_shader_program().bind_this();
	vertex_attribute& vertex_attribs = vertex_attribute::position3f_texcoord2f();
	//绘制到屏幕，绑定顺序是VAO、VBO、EBO，最后调用glVertexAttribPointer()设置顶点属性的格式
	gl_BindVertexArray(frame_vao);
	gl_BindBuffer(GL_ARRAY_BUFFER, frame_vbo);
	gl_BufferData(GL_ARRAY_BUFFER, 4 * vertex_attribs.get_vertex_size(), vertex_arr, GL_STATIC_DRAW);
	gl_BindBuffer(GL_ELEMENT_ARRAY_BUFFER, frame_ebo);
	gl_BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(full_screen_ndc_vertices_elements), full_screen_ndc_vertices_elements, GL_STATIC_DRAW);
	vertex_attribs.load(frame_vao);
	gl_BindTexture(GL_TEXTURE_2D, color_buffer);
	gl_DrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
	gl_Enable(GL_DEPTH_TEST);
}

float framebuffer::full_screen_ndc_vertices_pt_data[] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};
unsigned int framebuffer::full_screen_ndc_vertices_elements[4] = {0, 1, 2, 3};
