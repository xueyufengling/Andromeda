#include <andromeda/graphics/texture_2d.h>

#include <andromeda/util/string_utils.h>

using namespace andromeda::graphics;
using namespace andromeda::util;

void Texture2D::load(int level,bool generate_minimap,bool release_image) //实际加载图片进OpenGL内存，加载后image的数据即可释放，需要手动调用releaseImage();
{
	if(!texture_id) //texture_id=0才加载
		return;
	if(image.getPixelDataPtr())
	{
		glGenTextures(1,&texture_id);
		use();
		glTexImage2D(GL_TEXTURE_2D,level,GL_RGBA,image.getWidth(),image.getHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,image);
		if(generate_minimap)
			glGenerateMipmap(GL_TEXTURE_2D);
		if(release_image)
			releaseImage();
	}
}

void Texture2D::load(int texture_unit,int level,bool generate_minimap,bool release_image)
{
	if(!texture_id)
		return;
	if(image.getPixelDataPtr())
	{
		glGenTextures(1,&texture_id);
		use(texture_unit);
		glTexImage2D(GL_TEXTURE_2D,level,GL_RGBA,image.getWidth(),image.getHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,image);
		if(generate_minimap)
			glGenerateMipmap(GL_TEXTURE_2D);
		if(release_image)
			releaseImage();
	}
}

int Texture2D::max_texture_unit=0;

TextureType andromeda::graphics::parseTextureType(const char *type)
{
	char sec=uppercase(type[1]);
	switch(uppercase(type[0]))
	{
	case 'C':
		if(sec=='O')
			return TextureType::COLOR_MAP;
		if(sec=='C')
			return TextureType::CLEARCOAT_MAP;
		break;
	case 'D': //Diffuse为Color的别名
		if(sec=='F')
			return TextureType::COLOR_MAP;
		if(sec=='P')
			return TextureType::DISPLACEMENT_MAP;
		if(sec=='R')
			return TextureType::DIFFUSE_ROUGHNESS_MAP;
		break;
	case 'S':
		if(sec=='P')
			return TextureType::SPECULAR_MAP;
		if(sec=='H')
			return TextureType::SHININESS_MAP;
		if(sec=='E')
			return TextureType::SHEEN_MAP;
		break;
	case 'A':
		if(sec=='M')
			return TextureType::AMBIENT_MAP;
		if(sec=='O')
			return TextureType::AMBIENT_OCCLUSION_MAP;
		break;
	case 'E':
		if(sec=='M')
			return TextureType::EMISSIVE_MAP;
		if(sec=='C')
			return TextureType::EMISSION_COLOR_MAP;
		break;
	case 'H':
		if(sec=='E')
			return TextureType::HEIGHT_MAP;
		break;
	case 'N':
		if(sec=='M')
			return TextureType::NORMAL_MAP;
		if(sec=='C')
			return TextureType::NORMAL_CAMERA_MAP;
		break;
	case 'O':
		if(sec=='P')
			return TextureType::OPACITY_MAP;
		break;
	case 'L':
		if(sec=='I')
			return TextureType::LIGHT_MAP;
		break;
	case 'R':
		if(sec=='F')
			return TextureType::REFLECTION;
		break;
	case 'B':
		if(sec=='C')
			return TextureType::BASE_COLOR_MAP;
		break;
	case 'M':
		if(sec=='E')
			return TextureType::METALNESS_MAP;
		break;
	case 'T':
		if(sec=='N')
			return TextureType::TRANSMISSION_MAP;
		break;
	}
	return TextureType::UNKNOWN;
}
