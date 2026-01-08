#ifndef ANDROMEDA_GRAPHICS_COLORRGBA
#define ANDROMEDA_GRAPHICS_COLORRGBA

#include <string.h>
#include <stdint.h>
#include <math.h>

#include "../math/operations.h"

//一些颜色相关数学宏

#define getChannelInt(i) (i<0.0?0.0:(i<255.0?i/255.0:1.0))
#define getChannelFloat(f) (f<0.0?0.0:(f<1.0?f:1.0))
#define getPixelInt(i) (i<0?0:(i<255?i:255))
#define getPixelFloat(f) (f<0?0:(f<1?f*255:255))

#define getLightChannelInt(i) (i<0.0?0.0:(i<255.0?i/255.0:1.0))
#define getLightChannelFloat(f) (f<0.0?0.0:(f<1.0?f:1.0))
#define getChrominaceChannelInt(i) (i<-127.5?-0.5:(i>127.5?0.5:i/255))
#define getChrominaceChannelFloat(f) (f<-0.5?-0.5:(f>0.5?0.5:f))
#define getPixelLightInt(i) (i<0?0:(i<255?i:255))
#define getPixelLightFloat(f) (f<0?0:(f<1?f*255:255))
#define getPixelChrominaceInt(i) (i<-127.5?-127.5:(i>127.5?127.5:i))
#define getPixelChrominaceFloat(f) (f<-0.5?-0.5:(f>0.5?0.5:f))

namespace andromeda
{
namespace graphics
{
enum color_channel
{
	R = 0, G, B, A
};

class pixel;

class color_rgba
{
public:
	float r, g, b, a;

	operator pixel();

	color_rgba(int i_r, int i_g, int i_b, int i_a) :
			r(getChannelInt(i_r)), g(getChannelInt(i_g)), b(getChannelInt(i_b)), a(getChannelInt(i_a))
	{

	}

	color_rgba(float f_r = 0, float f_g = 0, float f_b = 0, float f_a = 0) :
			r(getChannelFloat(f_r)), g(getChannelFloat(f_g)), b(getChannelFloat(f_b)), a(getChannelFloat(f_a))
	{

	}

	color_rgba(pixel pixel);

	inline bool operator==(color_rgba destcolor)
	{
		return (r == destcolor.r && g == destcolor.g && b == destcolor.b && a == destcolor.a);
	}

	inline color_rgba& set_rgba(color_rgba color)
	{
		memcpy(this, &color, sizeof(color_rgba));
		return *this;
	}

	inline color_rgba& set_rgba(color_channel color_ch, float color_value)
	{
		*((float*)this + color_ch) = getChannelFloat(color_value);
		return *this;
	}

	inline float get_rgba(color_channel color_ch)
	{
		return *((float*)this + color_ch);
	}

	color_rgba& set_rgba(float _r = -1, float _g = -1, float _b = -1, float _a = -1); //设置-1表示不变，0-255则改变为设置的值，超出这个范围的就取0或255

	pixel to_pixel();

	static color_rgba mix_alpha(color_rgba below, color_rgba above);

	inline float gray_scale(float weight_r, float weight_g, float weight_b)
	{
		return (r * weight_r + g * weight_g + b * weight_b) / 3.0;
	}

	static color_rgba WHITE;
	static color_rgba BLACK;
	static color_rgba RED;
	static color_rgba GREEN;
	static color_rgba BLUE;
	static color_rgba YELLOW;
	static color_rgba PURPLE;
	static color_rgba CYAN;
	static color_rgba TRANSPARENT_BLACK;
};

class pixel
{
public:
	unsigned char r, g, b, a;

	operator color_rgba()
	{
		return color_rgba(r, g, b, a);
	}

	pixel(unsigned char i_r = 0, unsigned char i_g = 0, unsigned char i_b = 0, unsigned char i_a = 0) :
			r(i_r), g(i_g), b(i_b), a(i_a)
	{

	}

	pixel(float f_r, float f_g, float f_b, float f_a) :
			r(getPixelFloat(f_r)), g(getPixelFloat(f_g)), b(getPixelFloat(f_b)), a(getPixelFloat(f_a))
	{

	}

	pixel(color_rgba color) :
			r(getPixelFloat(color.r)), g(getPixelFloat(color.g)), b(getPixelFloat(color.b)), a(getPixelFloat(color.a))
	{

	}

	inline bool operator==(pixel destpixel)
	{
		return (r == destpixel.r && g == destpixel.g && b == destpixel.b && a == destpixel.a);
	}

	color_rgba to_color_rgba()
	{
		return color_rgba(r, g, b, a);
	}

	inline uint32_t pack_to_int()
	{
		return (((((((uint32_t)r) << 8) | g) << 8) | b) << 8) | a;
	}

	inline pixel& set_rgba(pixel pixel)
	{
		memcpy(this, &pixel, sizeof(pixel));
		return *this;
	}

	inline pixel& set_rgba(color_channel color_ch, float color_value)
	{
		*((int*)this + color_ch) = getChannelInt(color_value);
		return *this;
	}

	inline int get_rgba(color_channel color_ch)
	{
		return *((int*)this + color_ch);
	}

	pixel& set_rgba(int _r = -1, int _g = -1, int _b = -1, int _a = -1); //设置-1表示不变，0-255则改变为设置的值，超出这个范围的就取0或255

	inline pixel& mul_factor(float f)
	{
		r *= f;
		g *= f;
		b *= f;
		return *this;
	}

	static pixel WHITE;
	static pixel BLACK;
	static pixel RED;
	static pixel GREEN;
	static pixel BLUE;
	static pixel YELLOW;
	static pixel PURPLE;
	static pixel CYAN;
	static pixel TRANSPARENT_BLACK;
};
}
}

#endif //ANDROMEDA_GRAPHICS_COLORRGBA
