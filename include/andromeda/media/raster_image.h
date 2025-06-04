#ifndef ANDROMEDA_MEDIA_RASTERIMAGE
#define ANDROMEDA_MEDIA_RASTERIMAGE

#include "../graphics/color_rgba.h"
#include <malloc.h>

#include "image_factory.h"

namespace andromeda {
	namespace media {
		class RasterImage
		{
		public:
			operator andromeda::graphics::Pixel*()
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

			RasterImage(int img_width=0,int img_height=0,andromeda::graphics::Pixel* data=nullptr,bool alloc_if_null=false); //alloc_if_null决定如果data指定为nullptr是否自动分配内存

			static RasterImage& readImage(const char* image_path);
			void writeImage(const char* image_path);

			RasterImage copyRasterImage();

			andromeda::graphics::Pixel* getPixelDataPtr()
			{
				return pixels;
			}

			inline int getWidth()
			{
				return width;
			}

			inline int getHeight()
			{
				return height;
			}

			void allocate();
			inline void release()
			{
				free((void*)pixels);
			}

			void setPixel(int x,int y,andromeda::graphics::Pixel new_pixel)
			{
				*(andromeda::graphics::Pixel*)(pixels+width*y+x)=new_pixel;
			}

			void setPixel(int x,int y,andromeda::graphics::ColorRGBA new_pixel_color)
			{
				*(andromeda::graphics::Pixel*)(pixels+width*y+x)=new_pixel_color.toPixel();
			}

			inline andromeda::graphics::Pixel& getPixel(int x,int y)
			{
				return *(pixels+width*y+x);
			}

			//Operations
			void fillRect(int start_x,int start_y,int end_x,int end_y,andromeda::graphics::Pixel pixel);
			void fillAll(andromeda::graphics::Pixel pixel);

			void mix(RasterImage& img,int pos_x,int pos_y);
			void mulAlphaFactor(float factor);

			static RasterImage& cutImage(RasterImage& src_img,int start_x,int start_y,int end_x,int end_y);

			void setPixelData(andromeda::graphics::Pixel* data,int img_width,int img_height,image_size_options op); //设置某个区域的图像，将覆盖区域原本的数据
		protected:
			int height=0,width=0;
			andromeda::graphics::Pixel* pixels=nullptr;
		};
	}
}

#endif // ANDROMEDA_MEDIA_RASTERIMAGE
