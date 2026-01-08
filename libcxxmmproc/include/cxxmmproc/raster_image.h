#ifndef ANDROMEDA_MEDIA_RASTERIMAGE
#define ANDROMEDA_MEDIA_RASTERIMAGE

#include "../graphics/color_rgba.h"
#include <malloc.h>
#include <functional>

extern "C"
{
struct GLFWimage;
}

namespace andromeda
{
namespace media
{
class raster_image
{
protected:
	int width = 0, height = 0;
	andromeda::graphics::pixel* pixels = nullptr;

public:
	typedef std::function<void(andromeda::graphics::pixel&)> traversal_func;

	enum edge_options : char
	{
		IMAGE_EDGE_INCLUDE, IMAGE_EDGE_ABANDON
	};
	enum size_options : char
	{
		IMAGE_SIZE_FIXED, IMAGE_SIZE_FIT
	};

	operator andromeda::graphics::pixel*()
	{
		return pixels;
	}

	operator unsigned char*()
	{
		return (unsigned char*)pixels;
	}

	operator void*()
	{
		return (void*)pixels;
	}

	operator const void*()
	{
		return (const void*)pixels;
	}

	operator GLFWimage*()
	{
		return (GLFWimage*)this;
	}

	raster_image(int img_width = 0, int img_height = 0, andromeda::graphics::pixel* data = nullptr, bool alloc_if_null = false); //alloc_if_null决定如果data指定为nullptr是否自动分配内存

	/**
	 * @brief 从文件读取图像
	 */
	static raster_image& read(const char* image_path);

	/**
	 * @brief 将图像写入文件
	 */
	void write(const char* image_path);

	raster_image copy();

	andromeda::graphics::pixel* get_pixels()
	{
		return pixels;
	}

	inline int get_width()
	{
		return width;
	}

	inline int get_height()
	{
		return height;
	}

	void allocate();

	inline void release()
	{
		free((void*)pixels);
		pixels = nullptr;
	}

	void set_pixel(int x, int y, andromeda::graphics::pixel new_pixel)
	{
		*(andromeda::graphics::pixel*)(pixels + width * y + x) = new_pixel;
	}

	void set_pixel(int x, int y, andromeda::graphics::color_rgba new_pixel_color)
	{
		*(andromeda::graphics::pixel*)(pixels + width * y + x) = new_pixel_color.to_pixel();
	}

	inline andromeda::graphics::pixel& get_pixel(int x, int y)
	{
		return *(pixels + width * y + x);
	}

	//Operations
	void fill_rect(int start_x, int start_y, int end_x, int end_y, andromeda::graphics::pixel pixel);

	void fill_all(andromeda::graphics::pixel pixel);

	void mix(raster_image& img, int pos_x, int pos_y);

	void mul_alpha(float factor);

	static raster_image& cut(raster_image& src_img, int start_x, int start_y, int end_x, int end_y);

	/**
	 * @brief 设置某个区域的图像，将覆盖区域原本的数据
	 */
	void set_pixels(andromeda::graphics::pixel* data, int img_width, int img_height, size_options op);
};

andromeda::graphics::pixel* generatePixelData(int height, int width, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255);
void fillPixelData(void* data, int height, int width, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255);
int traversal_pixels(raster_image& image, raster_image::traversal_func func, int x = 0, int y = 0, int stride_x = 0, int stride_y = 0, raster_image::edge_options op = raster_image::edge_options::IMAGE_EDGE_INCLUDE);

unsigned char* convertRGBA32ToRGB24(unsigned char* data, int width, int height, float br, float bg, float bb);
void convertRGBA32ToRGB24(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* dest);
void convertRGBA32ToRGB24(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* dest, int linesize);
void convertRGBA32ToYUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, int linesize_Y, signed char* U, int linesize_U, signed char* V, int linesize_V);
char** convertRGBA32ToYUV444P(unsigned char* data, int width, int height, float br, float bg, float bb);
void convertRGBA32ToYUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, signed char* U, signed char* V);
void convertYUV444PToRGBA32(int width, int height, unsigned char* Y, int linesize_Y, signed char* U, int linesize_U, signed char* V, int linesize_V, unsigned char* dest);
void convertYUV444PToRGBA32(int width, int height, unsigned char* Y, signed char* U, signed char* V, unsigned char* dest);

void convertRGBA32Tou255YUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, int linesize_Y, unsigned char* U, int linesize_U, unsigned char* V, int linesize_V);
unsigned char** convertRGBA32Tou255YUV444P(unsigned char* data, int width, int height, float br, float bg, float bb);
void convertRGBA32Tou255YUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, unsigned char* U, unsigned char* V);
void convertu255YUV444PToRGBA32(int width, int height, unsigned char* Y, int linesize_Y, unsigned char* U, int linesize_U, unsigned char* V, int linesize_V, unsigned char* dest);
void convertu255YUV444PToRGBA32(int width, int height, unsigned char* Y, unsigned char* U, unsigned char* V, unsigned char* dest);

raster_image& getRasterImageByu255YUV444P(int width, int height, unsigned char* Y, int linesize_Y, unsigned char* U, int linesize_U, unsigned char* V, int linesize_V);

}
}

#endif // ANDROMEDA_MEDIA_RASTERIMAGE
