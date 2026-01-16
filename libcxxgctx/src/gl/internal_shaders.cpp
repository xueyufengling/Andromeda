#include <cxxgctx/gl/internal_shaders.h>

#include <cxxgctx/gl/vertex_attribute.h>

namespace cxxgctx
{
namespace gl
{
//传递顶点着色器
const char* pct_pass_vertex_shader =
		"#version 330 core\n"
				"layout (location=0) in vec3 " VERTEX_ATTRIB_NAME_POSITION ";\n"
		"layout (location=1) in vec4 " VERTEX_ATTRIB_NAME_COLOR ";\n"
		"layout (location=2) in vec2 " VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"out vec4 vertex_" VERTEX_ATTRIB_NAME_COLOR ";\n"
		"out vec2 vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"void main()\n"
		"{\n"
		"	gl_Position=vec4(" VERTEX_ATTRIB_NAME_POSITION ",1.0);\n"
		"	vertex_" VERTEX_ATTRIB_NAME_COLOR "=" VERTEX_ATTRIB_NAME_COLOR ";\n"
		"	vertex_" VERTEX_ATTRIB_NAME_TEXCOORD "=" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"}";

const char* pct_pass_vertex_instanced_shader =
		"#version 330 core\n"
				"layout (location=0) in vec3 " VERTEX_ATTRIB_NAME_POSITION ";\n"
		"layout (location=1) in vec4 " VERTEX_ATTRIB_NAME_COLOR ";\n"
		"layout (location=2) in vec2 " VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"layout (location=3) in mat4 " VERTEX_ATTRIB_NAME_MODEL_MATRIX ";\n"
		"out vec4 vertex_" VERTEX_ATTRIB_NAME_COLOR ";\n"
		"out vec2 vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"void main()\n"
		"{\n"
		"	gl_Position=" VERTEX_ATTRIB_NAME_MODEL_MATRIX "*vec4(" VERTEX_ATTRIB_NAME_POSITION ",1.0);\n"
		"	vertex_" VERTEX_ATTRIB_NAME_COLOR "=" VERTEX_ATTRIB_NAME_COLOR ";\n"
		"	vertex_" VERTEX_ATTRIB_NAME_TEXCOORD "=" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"}";

//默认片段着色器
const char* pct_default_fragment_shader =
		"#version 330 core\n"
				"in vec4 vertex_" VERTEX_ATTRIB_NAME_COLOR ";\n"
		"in vec2 vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"out vec4 fragment_color;\n"
		"uniform sampler2D texture_2d;\n"
		"void main()\n"
		"{\n"
		"	fragment_color=texture(texture_2d,vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ")*vertex_" VERTEX_ATTRIB_NAME_COLOR ";\n"
		"}";

//Framebuffer传递顶点着色器
const char* pt_pass_vertex_shader =
		"#version 330 core\n"
				"layout (location=0) in vec3 " VERTEX_ATTRIB_NAME_POSITION ";\n"
		"layout (location=1) in vec2 " VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"out vec2 vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"void main()\n"
		"{\n"
		"	gl_Position=vec4(" VERTEX_ATTRIB_NAME_POSITION ",1.0);\n"
		"	vertex_" VERTEX_ATTRIB_NAME_TEXCOORD "=" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"}";

const char* pt_pass_vertex_instanced_shader =
		"#version 330 core\n"
				"layout (location=0) in vec3 " VERTEX_ATTRIB_NAME_POSITION ";\n"
		"layout (location=1) in vec2 " VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"layout (location=2) in mat4 " VERTEX_ATTRIB_NAME_MODEL_MATRIX ";\n"
		"out vec2 vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"void main()\n"
		"{\n"
		"	gl_Position=" VERTEX_ATTRIB_NAME_MODEL_MATRIX "*vec4(" VERTEX_ATTRIB_NAME_POSITION ",1.0);\n"
		"	vertex_" VERTEX_ATTRIB_NAME_TEXCOORD "=" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"}";

//Framebuffer默认片段着色器
const char* pt_default_fragment_shader =
		"#version 330 core\n"
				"in vec2 vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"out vec4 fragment_color;\n"
		"uniform sampler2D texture_2d;\n"
		"void main()\n"
		"{\n"
		"	fragment_color=texture(texture_2d,vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ");\n"
		"}";

const char* pt_transparent_fix_fragment_shader =
		"#version 330 core\n"
				"in vec2 vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ";\n"
		"out vec4 fragment_color;\n"
		"uniform sampler2D texture_2d;\n"
		"vec3 black_color=vec3(0,0,0);\n"
		"vec3 fixed_black_color=vec3(1/255,1/255,1/255);\n"
		"void main()\n"
		"{\n"
		"	fragment_color=texture(texture_2d,vertex_" VERTEX_ATTRIB_NAME_TEXCOORD ");\n"
		"	if(fragment_color.rgb==black_color)\n"
		"		fragment_color.rgb=fixed_black_color;\n"
		"}";
}
}
