#ifndef ANDROMEDA_MEDIA_IMAGEFACTORY
#define ANDROMEDA_MEDIA_IMAGEFACTORY

#include <fstream>

#include "../graphics/color_rgba.h"

namespace andromeda {
	namespace media {
		class RasterImage;

		typedef void (*traversal_func)(andromeda::graphics::Pixel&);

		enum image_edge_options : char
		{
			IMAGE_EDGE_INCLUDE, IMAGE_EDGE_ABANDON
		};
		enum image_size_options : char
		{
			IMAGE_SIZE_FIXED, IMAGE_SIZE_FIT
		};

		andromeda::graphics::Pixel* generatePixelData(int height,int width,unsigned char r=0,unsigned char g=0,unsigned char b=0,unsigned char a=255);
		void fillPixelData(void* data,int height,int width,unsigned char r=0,unsigned char g=0,unsigned char b=0,unsigned char a=255);
		int traversalPixels(RasterImage& image,traversal_func func,int x=0,int y=0,int stride_x=0,int stride_y=0,image_edge_options op=IMAGE_EDGE_INCLUDE);

		unsigned char* convertRGBA32ToRGB24(unsigned char* data,int width,int height,float br,float bg,float bb);
		void convertRGBA32ToRGB24(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* dest);
		void convertRGBA32ToRGB24(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* dest,int linesize);
		void convertRGBA32ToYUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,int linesize_Y,signed char* U,int linesize_U,signed char* V,int linesize_V);
		char** convertRGBA32ToYUV444P(unsigned char* data,int width,int height,float br,float bg,float bb);
		void convertRGBA32ToYUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,signed char* U,signed char* V);
		void convertYUV444PToRGBA32(int width,int height,unsigned char* Y,int linesize_Y,signed char* U,int linesize_U,signed char* V,int linesize_V,unsigned char* dest);
		void convertYUV444PToRGBA32(int width,int height,unsigned char* Y,signed char* U,signed char* V,unsigned char* dest);

		void convertRGBA32Tou255YUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,int linesize_Y,unsigned char* U,int linesize_U,unsigned char* V,int linesize_V);
		unsigned char** convertRGBA32Tou255YUV444P(unsigned char* data,int width,int height,float br,float bg,float bb);
		void convertRGBA32Tou255YUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,unsigned char* U,unsigned char* V);
		void convertu255YUV444PToRGBA32(int width,int height,unsigned char* Y,int linesize_Y,unsigned char* U,int linesize_U,unsigned char* V,int linesize_V,unsigned char* dest);
		void convertu255YUV444PToRGBA32(int width,int height,unsigned char* Y,unsigned char* U,unsigned char* V,unsigned char* dest);

		RasterImage& getRasterImageByu255YUV444P(int width,int height,unsigned char* Y,int linesize_Y,unsigned char* U,int linesize_U,unsigned char* V,int linesize_V);
	}
}

#endif // ANDROMEDA_MEDIA_IMAGEFACTORY
