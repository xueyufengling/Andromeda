#include <andromeda/graphics/framebuffer.h>

#include <andromeda/graphics/shader_program.h>
#include <andromeda/graphics/internal_shaders.h>
#include <andromeda/graphics/vertex_attribute.h>
#include <andromeda/util/log.h>

using namespace andromeda::graphics;

bool framebuffer::alloc(int try_timeout)
{
	ALLOC: if(allocated)
		clear_all_buffers();
	else
	{
		glGenFramebuffers(1, &gl_id); //生成帧缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, gl_id);
		glGenTextures(1, &color_buffer); //生成颜色缓冲，可读可写
		glBindTexture(GL_TEXTURE_2D, color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FRAMEBUFFER_BUFFER_SIZE(width), FRAMEBUFFER_BUFFER_SIZE(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //为颜色缓冲分配内存
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //设置颜色缓冲采样格式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0); //将颜色缓冲绑定到帧缓冲gl_id
		glGenRenderbuffers(1, &depth_stencil_buffer); //生成渲染缓冲的深度、模板缓冲部件，GL_RENDERBUFFER效率比GL_TEXTURE_2D更高，该缓冲是只写的
		glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FRAMEBUFFER_BUFFER_SIZE(width), FRAMEBUFFER_BUFFER_SIZE(height)); //为深度模板缓冲分配内存
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_buffer); //将深度模板缓冲绑定到gl_id
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //解绑帧缓冲
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glGenBuffers(1, &frame_vbo);
		glGenBuffers(1, &frame_ebo);
		glGenVertexArrays(1, &frame_vao);
		--try_timeout;
	}
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //如果分配失败或者不完整
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
	glDeleteFramebuffers(1, &framebuffer_id);
	GLuint color_buffer = get_framebuffer_texture(framebuffer_id, 0);
	glDeleteTextures(1, &color_buffer);
	GLuint depth_stencil_buffer = get_framebuffer_depth_stencil(framebuffer_id);
	glDeleteRenderbuffers(1, &depth_stencil_buffer);
}

GLuint framebuffer::get_framebuffer_texture(GLuint dest_framebuffer_id, int texture_attachment) //传入颜色缓冲序号
{
	GLint dest_texture;
	glBindFramebuffer(GL_FRAMEBUFFER, dest_framebuffer_id);
	if(dest_framebuffer_id) //用户创建缓冲
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &dest_texture);
	else
		//默认缓冲
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_FRONT_LEFT + texture_attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &dest_texture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return dest_texture;
}

GLuint framebuffer::get_framebuffer_depth_stencil(GLuint dest_framebuffer_id) //传入颜色缓冲序号
{
	GLint dest_depth_stencil;
	glBindFramebuffer(GL_FRAMEBUFFER, dest_framebuffer_id);
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &dest_depth_stencil);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return dest_depth_stencil;
}

void framebuffer::copy_color_buffer(GLuint dest_framebuffer_id, int color_buffer_attachment)
{
	if(dest_framebuffer_id)
	{
		GLuint dest_texture = get_framebuffer_texture(dest_framebuffer_id, color_buffer_attachment);
		glBindFramebuffer(GL_FRAMEBUFFER, dest_framebuffer_id);
		glBindTexture(GL_TEXTURE_2D, dest_texture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
		blit_to_screen();
}

void framebuffer::render_to_screen(float* vertex_arr)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //绑定到屏幕帧缓冲
	glDisable(GL_DEPTH_TEST);
	clear_color_buffer();
	andromeda::graphics::pt_default_shader_program().use();
	vertex_attribute& vertex_attribs = vertex_attribute::position3f_texcoord2f();
	//绘制到屏幕，绑定顺序是VAO、VBO、EBO，最后调用glVertexAttribPointer()设置顶点属性的格式
	glBindVertexArray(frame_vao);
	glBindBuffer(GL_ARRAY_BUFFER, frame_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * vertex_attribs.get_vertex_size(), vertex_arr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frame_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(full_screen_ndc_vertices_elements), full_screen_ndc_vertices_elements, GL_STATIC_DRAW);
	vertex_attribs.load(frame_vao);
	glBindTexture(GL_TEXTURE_2D, color_buffer);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
	glEnable(GL_DEPTH_TEST);
}

float framebuffer::full_screen_ndc_vertices_pt_data[] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};
unsigned int framebuffer::full_screen_ndc_vertices_elements[4] = {0, 1, 2, 3};
