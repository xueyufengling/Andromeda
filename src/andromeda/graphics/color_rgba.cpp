#include <andromeda/graphics/color_rgba.h>

#include <andromeda/media/raster_image.h>
#include <andromeda/util/typedefs.h>

using namespace andromeda::graphics;

color_rgba color_rgba::WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
color_rgba color_rgba::BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
color_rgba color_rgba::RED = {1.0f, 0.0f, 0.0f, 1.0f};
color_rgba color_rgba::GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
color_rgba color_rgba::BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
color_rgba color_rgba::YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
color_rgba color_rgba::PURPLE = {1.0f, 0.0f, 1.0f, 1.0f};
color_rgba color_rgba::CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
color_rgba color_rgba::TRANSPARENT_BLACK = {0.0f, 0.0f, 0.0f, 0.0f};

pixel pixel::WHITE = {u8(255), u8(255), u8(255), u8(255)};
pixel pixel::BLACK = {u8(0), u8(0), u8(0), u8(255)};
pixel pixel::RED = {u8(255), u8(0), u8(0), u8(255)};
pixel pixel::GREEN = {u8(0), u8(255), u8(0), u8(255)};
pixel pixel::BLUE = {u8(0), u8(0), u8(255), u8(255)};
pixel pixel::YELLOW = {u8(255), u8(255), u8(0), u8(255)};
pixel pixel::PURPLE = {u8(255), u8(0), u8(255), u8(255)};
pixel pixel::CYAN = {u8(0), u8(255), u8(255), u8(255)};
pixel pixel::TRANSPARENT_BLACK = {u8(0), u8(0), u8(0), u8(0)};

color_rgba::color_rgba(const pixel pixel) :
		r(getChannelInt(pixel.r)), g(getChannelInt(pixel.g)), b(getChannelInt(pixel.b)), a(getChannelInt(pixel.a))
{
}

color_rgba::operator pixel()
{
	return pixel(r, g, b, a);
}

pixel color_rgba::to_pixel()
{
	return pixel(r, g, b, a);
}

color_rgba& color_rgba::set_rgba(float _r, float _g, float _b, float _a) //设置-1表示不变，0-255则改变为设置的值，超出这个范围的就取0或255
{
	if(_r != -1)
		r = getPixelFloat(_r);
	if(_g != -1)
		g = getPixelFloat(_g);
	if(_b != -1)
		b = getPixelFloat(_b);
	if(_a != -1)
		a = getPixelFloat(_a);
	return *this;
}

color_rgba color_rgba::mix_alpha(color_rgba below, color_rgba above)
{
	color_rgba alpha_big, alpha_less;
	if(below.a > above.a)
	{
		alpha_big = below;
		alpha_less = above;
	}
	else if(below.a < above.a)
	{
		alpha_big = above;
		alpha_less = below;
	}
	else
	{
		if(below.a == 1 && above.a == 1)
			return above;
		return color_rgba((below.r + above.r) / 2, (below.g + above.g) / 2, (below.b + above.b) / 2, below.a);
	}
	float unit = alpha_big.a;
	float rate = alpha_less.a / alpha_big.a;
	return color_rgba(alpha_less.r * rate * unit + alpha_big.r * (1 - rate) * unit, alpha_less.g * rate * unit + alpha_big.g * (1 - rate) * unit, alpha_less.b * rate * unit + alpha_big.b * (1 - rate) * unit, unit);
}

//Pixel
pixel& pixel::set_rgba(int _r, int _g, int _b, int _a) //设置-1表示不变，0-255则改变为设置的值，超出这个范围的就取0或255
{
	if(_r != -1)
		r = getPixelInt(_r);
	if(_g != -1)
		g = getPixelInt(_g);
	if(_b != -1)
		b = getPixelInt(_b);
	if(_a != -1)
		a = getPixelInt(_a);
	return *this;
}
