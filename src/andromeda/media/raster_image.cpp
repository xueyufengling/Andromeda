#include <andromeda/media/raster_image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <string.h>
#include <andromeda/media/image_process.h>
#include <andromeda/media/image_factory.h>

using namespace andromeda::media;
using namespace andromeda::graphics;

RasterImage::RasterImage(int img_width,int img_height,Pixel* data,bool alloc_if_null) :
		height(img_height), width(img_width), pixels(data)
{
	if(alloc_if_null&&!data)
		pixels=(Pixel*)malloc(sizeof(Pixel)*img_width*img_height);
}

RasterImage RasterImage::copyRasterImage()
{
	int len=height*width;
	Pixel* cpy=(Pixel*)malloc(sizeof(Pixel)*len);
	memcpy(cpy,pixels,sizeof(Pixel)*len);
	return RasterImage(width,height,cpy);
}

void RasterImage::allocate()
{
	pixels=(Pixel*)malloc(sizeof(Pixel)*width*height);
}

RasterImage& RasterImage::readImage(const char* image_path)
{
	RasterImage* image=new RasterImage();
	image->pixels=(Pixel*)stbi_load(image_path,&image->width,&image->height,nullptr,4);
	return *image;
}

void RasterImage::writeImage(const char* image_path)
{
	stbi_write_png(image_path,width,height,4,pixels,4*width);
}

void RasterImage::fillRect(int start_x,int start_y,int end_x,int end_y,Pixel pixel)
{
	RasterImage& this_tmage=*this;
	startTraversalPixelsInArea(this_tmage,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
			this_pixel_in_area=pixel;
		endTraversalPixelsInArea(this_tmage,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
}

void RasterImage::fillAll(Pixel pixel)
{
	fillRect(0,0,width-1,height-1,pixel);
}

void RasterImage::mix(RasterImage& img,int pos_x,int pos_y)
{
	int traversal_width=pos_x+img.width>width?width-pos_x:img.width;
	int traversal_height=pos_y+img.height>height?height-pos_y:img.height;
	startTraversalPixelsInArea(img,0,0,traversal_width,traversal_height,0,0,IMAGE_EDGE_INCLUDE)
			Pixel& pixel_in_src_image=this->getPixel(pos_x+absolute_x_in_area,pos_y+absolute_y_in_area);
			pixel_in_src_image=ColorRGBA::mixAlpha(pixel_in_src_image.toColorRGBA(),this_pixel_in_area.toColorRGBA()).toPixel();
		endTraversalPixelsInArea(img,0,0,traversal_width,traversal_height,0,0,IMAGE_EDGE_INCLUDE)
}

void RasterImage::mulAlphaFactor(float factor)
{
	RasterImage& this_image=*this;
	startTraversalPixels(this_image,0,0,0,0,IMAGE_EDGE_INCLUDE)
			this_pixel_in_image.a*=factor;
		endTraversalPixels(this_image,0,0,0,0,IMAGE_EDGE_INCLUDE)
}

RasterImage& RasterImage::cutImage(RasterImage& src_img,int start_x,int start_y,int end_x,int end_y)
{
	RasterImage* result=new RasterImage(end_x-start_x,end_y-start_y);
	result->allocate();
	startTraversalPixelsInArea(src_img,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
			result->setPixel(relative_x_in_area,relative_y_in_area,this_pixel_in_area);
		endTraversalPixelsInArea(src_img,start_x,start_y,end_x,end_y,0,0,IMAGE_EDGE_INCLUDE)
	return *result;
}

void RasterImage::setPixelData(Pixel* data,int img_width,int img_height,image_size_options op)
{
	switch(op)
	{
	case IMAGE_SIZE_FIXED:

		break;
	case IMAGE_SIZE_FIT:

		break;
	}
}

