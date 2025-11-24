#ifndef ANDROMEDA_GRAPHICS_TEXTURE2D
#define ANDROMEDA_GRAPHICS_TEXTURE2D

#include "gl_basic.h"
#include "../media/raster_image.h"

namespace andromeda
{
namespace graphics
{
enum texture_type : int //材质类型，与aiTextureType保持一致
{
	NONE = 0, //无贴图
	COLOR_MAP = 1,
	DIFFUSE_MAP = 1,
	SPECULAR_MAP,
	AMBIENT_MAP,
	EMISSIVE_MAP,
	HEIGHT_MAP,
	NORMAL_MAP,
	SHININESS_MAP,
	OPACITY_MAP,
	DISPLACEMENT_MAP,
	LIGHT_MAP,
	REFLECTION, //通用贴图
	BASE_COLOR_MAP = 12,
	NORMAL_CAMERA_MAP,
	EMISSION_COLOR_MAP,
	METALNESS_MAP,
	DIFFUSE_ROUGHNESS_MAP,
	AMBIENT_OCCLUSION_MAP, //PBR贴图
	UNKNOWN = 18, //未知贴图
	SHEEN_MAP = 19,
	CLEARCOAT_MAP,
	TRANSMISSION_MAP
};

//UV坐标，左下角为原点，向上向右为正
typedef struct uv_coord
{
	float u1, v1, u2, v2; //uv1为左下角，uv2为右上角
} uv_coord;

//像素坐标，左上角为原点，向右向下为正
typedef struct pixel_coord
{
	int x1, y1, x2, y2; //uv1为左下角，uv2为右上角
} pixel_coord;

texture_type parse_texture_type(const char* type);

class texture2d : public andromeda::graphics::gl_component<texture2d>
{
private:
	const char* texture_path = nullptr;
	texture_type type = texture_type::NONE;
	andromeda::media::raster_image image;
	static int max_texture_unit; //OpenGL支持的最大纹理单元数目，0表示未初始化

public:
	__attribute__((always_inline)) inline texture2d(GLuint exist_texture_id, texture_type type = texture_type::UNKNOWN) :
			gl_component<texture2d>(exist_texture_id), type(type)
	{
	}

	__attribute__((always_inline)) inline texture2d() = default;

	texture2d(andromeda::media::raster_image img, texture_type type = texture_type::UNKNOWN) :
			image(img), type(type)
	{
	}

	texture2d(const char* img_path, texture_type type = texture_type::UNKNOWN) :
			type(type)
	{
		set_image(img_path);
	}

	__attribute__((always_inline)) texture_type get_texture_type()
	{
		return type;
	}

	__attribute__((always_inline)) const char* get_texture_path()
	{
		return texture_path;
	}

	__attribute__((always_inline)) static inline int get_max_texture_unit()
	{
		if(!max_texture_unit)
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_unit);
		return max_texture_unit;
	}

	__attribute__((always_inline)) inline int get_width()
	{
		return image.get_width();
	}

	__attribute__((always_inline)) inline int get_height()
	{
		return image.get_height();
	}

	__attribute__((always_inline)) inline void set_image(andromeda::media::raster_image img)
	{
		image = img;
	}

	__attribute__((always_inline)) inline void set_image(const char* img_path)
	{
		image = andromeda::media::raster_image::read(img_path);
		texture_path = img_path;
	}

	/**
	 * 释放之后load()会返回false，需要重新调用set_image()设置数据才可加载新的纹理
	 */
	__attribute__((always_inline)) inline void release_image()
	{
		image.release();
	}

	__attribute__((always_inline)) inline texture2d& set_parameter(GLint para_name, GLint para_value) //设置之前先调用use()
	{
		glTexParameteri(GL_TEXTURE_2D, para_name, para_value);
		return *this;
	}

	/**
	 * @brief 实际加载图片进OpenGL内存，加载后image的数据即可释放，release_image决定是否调用release_image();load()加载后gl_id不为0
	 * @return 是否加载成功
	 */
	bool load(int level = 0, bool generate_minimap = true, bool release_image = true);

	bool load(int texture_unit, int level = 0, bool generate_minimap = true, bool release_image = true);

	__attribute__((always_inline)) inline static void bind(GLuint texture_id)
	{
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}

	__attribute__((always_inline)) inline static void bind(GLuint texture_id, int texture_unit)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}

	/**
	 * @brief 返回当前OpenGL绑定的Texture2D的id，查询代价高昂避免频繁调用
	 */
	__attribute__((always_inline)) inline static GLuint current_bind()
	{
		return gl_get_integer(GL_TEXTURE_BINDING_2D);
	}
};
}
}

#endif//ANDROMEDA_GRAPHICS_TEXTURE2D
