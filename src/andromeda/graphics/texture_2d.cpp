#include <andromeda/graphics/texture_2d.h>

#include <andromeda/util/string_utils.h>

using namespace andromeda::graphics;
using namespace andromeda::util;

bool texture2d::load(int level, bool generate_minimap, bool release_image) //实际加载图片进OpenGL内存，加载后image的数据即可释放，需要手动调用releaseImage();
{
	if(!gl_id) //gl_id=0才加载
		return false;
	if(image.get_pixels())
	{
		glGenTextures(1, &gl_id);
		use();
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, image.get_width(), image.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		if(generate_minimap)
			glGenerateMipmap(GL_TEXTURE_2D);
		if(release_image)
			this->release_image();
		return true;
	}
	else
	{
		return false;
	}
}

bool texture2d::load(int texture_unit, int level, bool generate_minimap, bool release_image)
{
	if(!gl_id)
		return false;
	if(image.get_pixels())
	{
		glGenTextures(1, &gl_id);
		use(texture_unit);
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, image.get_width(), image.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		if(generate_minimap)
			glGenerateMipmap(GL_TEXTURE_2D);
		if(release_image)
			this->release_image();
		return true;
	}
	else
	{
		return false;
	}
}

int texture2d::max_texture_unit = 0;

texture_type andromeda::graphics::parse_texture_type(const char* type)
{
	char sec = uppercase(type[1]);
	switch(uppercase(type[0]))
	{
	case 'C':
		if(sec == 'O')
			return texture_type::COLOR_MAP;
		if(sec == 'C')
			return texture_type::CLEARCOAT_MAP;
		break;
	case 'D': //Diffuse为Color的别名
		if(sec == 'F')
			return texture_type::COLOR_MAP;
		if(sec == 'P')
			return texture_type::DISPLACEMENT_MAP;
		if(sec == 'R')
			return texture_type::DIFFUSE_ROUGHNESS_MAP;
		break;
	case 'S':
		if(sec == 'P')
			return texture_type::SPECULAR_MAP;
		if(sec == 'H')
			return texture_type::SHININESS_MAP;
		if(sec == 'E')
			return texture_type::SHEEN_MAP;
		break;
	case 'A':
		if(sec == 'M')
			return texture_type::AMBIENT_MAP;
		if(sec == 'O')
			return texture_type::AMBIENT_OCCLUSION_MAP;
		break;
	case 'E':
		if(sec == 'M')
			return texture_type::EMISSIVE_MAP;
		if(sec == 'C')
			return texture_type::EMISSION_COLOR_MAP;
		break;
	case 'H':
		if(sec == 'E')
			return texture_type::HEIGHT_MAP;
		break;
	case 'N':
		if(sec == 'M')
			return texture_type::NORMAL_MAP;
		if(sec == 'C')
			return texture_type::NORMAL_CAMERA_MAP;
		break;
	case 'O':
		if(sec == 'P')
			return texture_type::OPACITY_MAP;
		break;
	case 'L':
		if(sec == 'I')
			return texture_type::LIGHT_MAP;
		break;
	case 'R':
		if(sec == 'F')
			return texture_type::REFLECTION;
		break;
	case 'B':
		if(sec == 'C')
			return texture_type::BASE_COLOR_MAP;
		break;
	case 'M':
		if(sec == 'E')
			return texture_type::METALNESS_MAP;
		break;
	case 'T':
		if(sec == 'N')
			return texture_type::TRANSMISSION_MAP;
		break;
	}
	return texture_type::UNKNOWN;
}
