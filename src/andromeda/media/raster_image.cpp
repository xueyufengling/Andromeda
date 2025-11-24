#include <andromeda/media/raster_image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <string.h>

using namespace andromeda::media;
using namespace andromeda::graphics;

raster_image::raster_image(int img_width, int img_height, pixel* data, bool alloc_if_null) :
		height(img_height), width(img_width), pixels(data)
{
	if(alloc_if_null && !data)
		pixels = (pixel*)malloc(sizeof(pixel) * img_width * img_height);
}

raster_image raster_image::copy()
{
	int len = height * width;
	pixel* cpy = (pixel*)malloc(sizeof(pixel) * len);
	memcpy(cpy, pixels, sizeof(pixel) * len);
	return raster_image(width, height, cpy);
}

void raster_image::allocate()
{
	pixels = (pixel*)malloc(sizeof(pixel) * width * height);
}

raster_image& raster_image::read(const char* image_path)
{
	raster_image* image = new raster_image();
	image->pixels = (pixel*)stbi_load(image_path, &image->width, &image->height, nullptr, 4);
	return *image;
}

void raster_image::write(const char* image_path)
{
	stbi_write_png(image_path, width, height, 4, pixels, 4 * width);
}

void raster_image::fill_rect(int start_x, int start_y, int end_x, int end_y, pixel pixel)
{
	raster_image& this_tmage = *this;
	//startTraversalPixelsInArea(this_tmage,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
	//		this_pixel_in_area=pixel;
	//	endTraversalPixelsInArea(this_tmage,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
}

void raster_image::fill_all(pixel pixel)
{
	for(int i; i < width * height; ++i)
		memcpy(pixels + 4 * i, &pixel, 4);
}

void raster_image::mix(raster_image& img, int pos_x, int pos_y)
{
	int traversal_width = pos_x + img.width > width ? width - pos_x : img.width;
	int traversal_height = pos_y + img.height > height ? height - pos_y : img.height;
	//startTraversalPixelsInArea(img,0,0,traversal_width,traversal_height,0,0,IMAGE_EDGE_INCLUDE)
	//		pixel& pixel_in_src_image=this->get_pixel(pos_x+absolute_x_in_area,pos_y+absolute_y_in_area);
	//		pixel_in_src_image=color_rgba::mix_alpha(pixel_in_src_image.to_color_rgba(),this_pixel_in_area.to_color_rgba()).toPixel();
	//	endTraversalPixelsInArea(img,0,0,traversal_width,traversal_height,0,0,IMAGE_EDGE_INCLUDE)
}

void raster_image::mul_alpha(float factor)
{
	raster_image& this_image = *this;
	//startTraversalPixels(this_image,0,0,0,0,IMAGE_EDGE_INCLUDE)
	//		this_pixel_in_image.a*=factor;
	//	endTraversalPixels(this_image,0,0,0,0,IMAGE_EDGE_INCLUDE)
}

raster_image& raster_image::cut(raster_image& src_img, int start_x, int start_y, int end_x, int end_y)
{
	raster_image* result = new raster_image(end_x - start_x, end_y - start_y);
	result->allocate();
	//startTraversalPixelsInArea(src_img,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
	//		result->setPixel(relative_x_in_area,relative_y_in_area,this_pixel_in_area);
	//	endTraversalPixelsInArea(src_img,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
	return *result;
}

void raster_image::set_pixels(pixel* data, int img_width, int img_height, size_options op)
{
	switch(op)
	{
	case IMAGE_SIZE_FIXED:

		break;
	case IMAGE_SIZE_FIT:

		break;
	}
}

pixel* andromeda::media::generatePixelData(int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	pixel* data = (pixel*)malloc(sizeof(pixel) * width * height);
	fillPixelData(data, width, height, r, g, b, a);
	return data;
}

void andromeda::media::fillPixelData(void* data, int height, int width, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	pixel p(r, g, b, a);
	for(int i; i < width * height; ++i)
		memcpy(data + 4 * i, &p, 4);
}

int andromeda::media::traversal_pixels(raster_image& image, raster_image::traversal_func func, int x, int y, int stride_x, int stride_y, raster_image::edge_options op)
{
	register int w = image.get_width(), h = image.get_height(), traversaled_count = 0;
	register raster_image::edge_options exec_flag_x, exec_flag_y = op;
	LOOP_Y: if(stride_x > w)
		return 0;
	exec_flag_x = op;
	LOOP_X: pixel& pixel = image.get_pixel(x, y);
	func(pixel);
	++traversaled_count;
	++x += stride_x;
	if(x < w)
		goto LOOP_X;
	switch(exec_flag_x)
	{
	case raster_image::edge_options::IMAGE_EDGE_INCLUDE:
		x = w - 1;
		exec_flag_x = raster_image::edge_options::IMAGE_EDGE_ABANDON;
		goto LOOP_X;
		break;
	case raster_image::edge_options::IMAGE_EDGE_ABANDON:
		break;
	}
	x = 0;
	exec_flag_x = op;
	++y += stride_y;
	if(y < h)
		goto LOOP_Y;
	switch(exec_flag_y)
	{
	case raster_image::edge_options::IMAGE_EDGE_INCLUDE:
		y = h - 1;
		exec_flag_y = raster_image::edge_options::IMAGE_EDGE_ABANDON;
		goto LOOP_Y;
		break;
	case raster_image::edge_options::IMAGE_EDGE_ABANDON:
		break;
	}
	return traversaled_count;
}

void andromeda::media::convertRGBA32ToYUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, int linesize_Y, signed char* U, int linesize_U, signed char* V, int linesize_V)
{
	color_rgba back_color(getChannelFloat(br), getChannelFloat(bg), getChannelFloat(bb), 1.0f);
	color_rgba this_color;
	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			this_color = color_rgba::mix_alpha(back_color, ((pixel*)data + y * width + x)->to_color_rgba());
			*(Y + y * linesize_Y + x) = getPixelLightFloat(0.299 * this_color.r + 0.587 * this_color.g + 0.114 * this_color.b);
			*(U + y * linesize_U + x) = getPixelChrominaceFloat(-0.1687 * this_color.r - 0.3313 * this_color.g + 0.5 * this_color.b);
			*(V + y * linesize_V + x) = getPixelChrominaceFloat(0.5 * this_color.r - 0.4187 * this_color.g - 0.0813 * this_color.b);
		}
	}
}

void andromeda::media::convertRGBA32ToYUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, signed char* U, signed char* V)
{
	andromeda::media::convertRGBA32ToYUV444P(data, width, height, br, bg, bb, Y, width, U, width, V, width);
}

char** andromeda::media::convertRGBA32ToYUV444P(unsigned char* data, int width, int height, float br, float bg, float bb)
{
	char** bytes_arr = (char**)malloc(3 * sizeof(char*));
	unsigned char* Y = (unsigned char*)malloc(width * height);
	signed char* U = (signed char*)malloc(width * height);
	signed char* V = (signed char*)malloc(width * height);
	andromeda::media::convertRGBA32ToYUV444P(data, width, height, br, bg, bb, Y, U, V);
	*bytes_arr = (char*)Y;
	*(bytes_arr + 1) = (char*)U;
	*(bytes_arr + 2) = (char*)V;
	return bytes_arr;
}

void andromeda::media::convertRGBA32ToRGB24(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* dest, int linesize)
{
	color_rgba back_color(getChannelFloat(br), getChannelFloat(bg), getChannelFloat(bb), 1.0f);
	color_rgba this_color;
	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			this_color = color_rgba::mix_alpha(back_color, ((pixel*)(data + y * width + x))->to_color_rgba());
			*(dest + 3 * y * linesize + 3 * x) = getPixelFloat(this_color.r);
			*(dest + 3 * y * linesize + 3 * x + 1) = getPixelFloat(this_color.g);
			*(dest + 3 * y * linesize + 3 * x + 2) = getPixelFloat(this_color.b);
		}
	}
}

unsigned char* andromeda::media::convertRGBA32ToRGB24(unsigned char* data, int width, int height, float br, float bg, float bb)
{
	unsigned char* bytes = (unsigned char*)malloc(3 * width * height);
	andromeda::media::convertRGBA32ToRGB24(data, width, height, br, bg, bb, bytes);
	return bytes;
}

void andromeda::media::convertRGBA32ToRGB24(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* dest)
{
	andromeda::media::convertRGBA32ToRGB24(data, width, height, br, bg, bb, dest, width);
}

void andromeda::media::convertYUV444PToRGBA32(int width, int height, unsigned char* Y, int linesize_Y, signed char* U, int linesize_U, signed char* V, int linesize_V, unsigned char* dest)
{
	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			unsigned char y = *(Y + y * linesize_Y + x);
			signed char u = *(U + y * linesize_U + x);
			signed char v = *(V + y * linesize_V + x);
			*(dest + 4 * y * width + 4 * x) = getChannelFloat(y + 1.403 * v);
			*(dest + 4 * y * width + 4 * x + 1) = getChannelFloat(y - 0.344 * u - 0.714 * v);
			*(dest + 4 * y * width + 4 * x + 2) = getChannelFloat(y + 1.77 * u);
			*(dest + 4 * y * width + 4 * x + 3) = 255;
		}
	}
}

void andromeda::media::convertYUV444PToRGBA32(int width, int height, unsigned char* Y, signed char* U, signed char* V, unsigned char* dest)
{
	andromeda::media::convertYUV444PToRGBA32(width, height, Y, width, U, width, V, width, dest);
}

void andromeda::media::convertRGBA32Tou255YUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, int linesize_Y, unsigned char* U, int linesize_U, unsigned char* V, int linesize_V)
{
	color_rgba back_color(getChannelFloat(br), getChannelFloat(bg), getChannelFloat(bb), 1.0f);
	pixel this_pixel;
	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			this_pixel = color_rgba::mix_alpha(back_color, ((pixel*)data + y * width + x)->to_color_rgba()).to_pixel();
			*(Y + y * linesize_Y + x) = 0.299 * this_pixel.r + 0.587 * this_pixel.g + 0.114 * this_pixel.b;
			*(U + y * linesize_U + x) = -0.1687 * this_pixel.r - 0.3313 * this_pixel.g + 0.5 * this_pixel.b + 128;
			*(V + y * linesize_V + x) = 0.5 * this_pixel.r - 0.4187 * this_pixel.g - 0.0813 * this_pixel.b + 128;
		}
	}
}

unsigned char** andromeda::media::convertRGBA32Tou255YUV444P(unsigned char* data, int width, int height, float br, float bg, float bb)
{
	unsigned char** bytes_arr = (unsigned char**)malloc(3 * sizeof(char*));
	unsigned char* Y = (unsigned char*)malloc(width * height);
	unsigned char* U = (unsigned char*)malloc(width * height);
	unsigned char* V = (unsigned char*)malloc(width * height);
	andromeda::media::convertRGBA32Tou255YUV444P(data, width, height, br, bg, bb, Y, U, V);
	*bytes_arr = Y;
	*(bytes_arr + 1) = U;
	*(bytes_arr + 2) = V;
	return bytes_arr;
}

void andromeda::media::convertRGBA32Tou255YUV444P(unsigned char* data, int width, int height, float br, float bg, float bb, unsigned char* Y, unsigned char* U, unsigned char* V)
{
	andromeda::media::convertRGBA32Tou255YUV444P(data, width, height, br, bg, bb, Y, width, U, width, V, width);
}

void andromeda::media::convertu255YUV444PToRGBA32(int width, int height, unsigned char* Y, int linesize_Y, unsigned char* U, int linesize_U, unsigned char* V, int linesize_V, unsigned char* dest)
{
	for(int pos_y = 0; pos_y < height; ++pos_y)
	{
		for(int pos_x = 0; pos_x < width; ++pos_x)
		{
			unsigned char y = *(Y + pos_y * linesize_Y + pos_x);
			unsigned char u = *(U + pos_y * linesize_U + pos_x);
			unsigned char v = *(V + pos_y * linesize_V + pos_x);
			pixel& this_pixel = *((pixel*)dest + pos_y * width + pos_x);
			this_pixel.r = y + 1.403 * (v - 128);
			this_pixel.g = y - 0.343 * (u - 128) - 0.714 * (v - 128);
			this_pixel.b = y + 1.77 * (u - 128);
			this_pixel.a = 255;
		}
	}
}

void andromeda::media::convertu255YUV444PToRGBA32(int width, int height, unsigned char* Y, unsigned char* U, unsigned char* V, unsigned char* dest)
{
	andromeda::media::convertu255YUV444PToRGBA32(width, height, Y, width, U, width, V, width, dest);
}

raster_image& andromeda::media::getRasterImageByu255YUV444P(int width, int height, unsigned char* Y, int linesize_Y, unsigned char* U, int linesize_U, unsigned char* V, int linesize_V)
{
	unsigned char* data = (unsigned char*)malloc(sizeof(pixel) * width * height);
	convertu255YUV444PToRGBA32(width, height, Y, linesize_Y, U, linesize_U, V, linesize_V, data);
	raster_image* img = new raster_image(width, height, (pixel*)data);
	return *img;
}
