#include <andromeda/media/image_factory.h>

#include <andromeda/media/image_process.h>
#include <andromeda/media/raster_image.h>

using namespace andromeda::graphics;
using namespace andromeda::media;

Pixel* andromeda::media::generatePixelData(int width,int height,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	Pixel* data=(Pixel*)malloc(sizeof(Pixel)*width*height);
	fillPixelData(data,width,height,r,g,b,a);
	return data;
}

void andromeda::media::fillPixelData(void *data,int height,int width,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	Pixel p(r,g,b,a);
	for(int i;i<width*height;++i)
		memcpy(data+4*i,&p,4);
}

int andromeda::media::traversalPixels(RasterImage& image,traversal_func func,int x,int y,int stride_x,int stride_y,image_edge_options op)
{
	register int w=image.getWidth(),h=image.getHeight(),traversaled_count=0;
	register image_edge_options exec_flag_x,exec_flag_y=op;
	LOOP_Y:if(stride_x>w)
		return 0;
	exec_flag_x=op;
	LOOP_X:Pixel& pixel=image.getPixel(x,y);
	func(pixel);
	++traversaled_count;
	++x+=stride_x;
	if(x<w)
		goto LOOP_X;
	switch(exec_flag_x)
	{
	case IMAGE_EDGE_INCLUDE:
		x=w-1;
		exec_flag_x=IMAGE_EDGE_ABANDON;
		goto LOOP_X;
		break;
	case IMAGE_EDGE_ABANDON:
		break;
	}
	x=0;
	exec_flag_x=op;
	++y+=stride_y;
	if(y<h)
		goto LOOP_Y;
	switch(exec_flag_y)
	{
	case IMAGE_EDGE_INCLUDE:
		y=h-1;
		exec_flag_y=IMAGE_EDGE_ABANDON;
		goto LOOP_Y;
		break;
	case IMAGE_EDGE_ABANDON:
		break;
	}
	return traversaled_count;
}

void andromeda::media::convertRGBA32ToYUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,int linesize_Y,signed char* U,int linesize_U,signed char* V,int linesize_V)
{
	ColorRGBA back_color(getChannelFloat(br),getChannelFloat(bg),getChannelFloat(bb),1.0f);
	ColorRGBA this_color;
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			this_color=ColorRGBA::mixAlpha(back_color,((Pixel*)data+y*width+x)->toColorRGBA());
			*(Y+y*linesize_Y+x)=getPixelLightFloat(0.299*this_color.r+0.587*this_color.g+0.114*this_color.b);
			*(U+y*linesize_U+x)=getPixelChrominaceFloat(-0.1687*this_color.r-0.3313*this_color.g+0.5*this_color.b);
			*(V+y*linesize_V+x)=getPixelChrominaceFloat(0.5*this_color.r-0.4187*this_color.g-0.0813*this_color.b);
		}
	}
}

void andromeda::media::convertRGBA32ToYUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,signed char* U,signed char* V)
{
	andromeda::media::convertRGBA32ToYUV444P(data,width,height,br,bg,bb,Y,width,U,width,V,width);
}

char** andromeda::media::convertRGBA32ToYUV444P(unsigned char* data,int width,int height,float br,float bg,float bb)
{
	char** bytes_arr=(char**)malloc(3*sizeof(char*));
	unsigned char* Y=(unsigned char*)malloc(width*height);
	signed char* U=(signed char*)malloc(width*height);
	signed char* V=(signed char*)malloc(width*height);
	andromeda::media::convertRGBA32ToYUV444P(data,width,height,br,bg,bb,Y,U,V);
	*bytes_arr=(char*)Y;
	*(bytes_arr+1)=(char*)U;
	*(bytes_arr+2)=(char*)V;
	return bytes_arr;
}

void andromeda::media::convertRGBA32ToRGB24(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* dest,int linesize)
{
	ColorRGBA back_color(getChannelFloat(br),getChannelFloat(bg),getChannelFloat(bb),1.0f);
	ColorRGBA this_color;
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			this_color=ColorRGBA::mixAlpha(back_color,((Pixel*)(data+y*width+x))->toColorRGBA());
			*(dest+3*y*linesize+3*x)=getPixelFloat(this_color.r);
			*(dest+3*y*linesize+3*x+1)=getPixelFloat(this_color.g);
			*(dest+3*y*linesize+3*x+2)=getPixelFloat(this_color.b);
		}
	}
}

unsigned char* andromeda::media::convertRGBA32ToRGB24(unsigned char* data,int width,int height,float br,float bg,float bb)
{
	unsigned char* bytes=(unsigned char*)malloc(3*width*height);
	andromeda::media::convertRGBA32ToRGB24(data,width,height,br,bg,bb,bytes);
	return bytes;
}

void andromeda::media::convertRGBA32ToRGB24(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* dest)
{
	andromeda::media::convertRGBA32ToRGB24(data,width,height,br,bg,bb,dest,width);
}

void andromeda::media::convertYUV444PToRGBA32(int width,int height,unsigned char* Y,int linesize_Y,signed char* U,int linesize_U,signed char* V,int linesize_V,unsigned char* dest)
{
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			unsigned char y=*(Y+y*linesize_Y+x);
			signed char u=*(U+y*linesize_U+x);
			signed char v=*(V+y*linesize_V+x);
			*(dest+4*y*width+4*x)=getChannelFloat(y+1.403*v);
			*(dest+4*y*width+4*x+1)=getChannelFloat(y-0.344*u-0.714*v);
			*(dest+4*y*width+4*x+2)=getChannelFloat(y+1.77*u);
			*(dest+4*y*width+4*x+3)=255;
		}
	}
}

void andromeda::media::convertYUV444PToRGBA32(int width,int height,unsigned char* Y,signed char* U,signed char* V,unsigned char* dest)
{
	andromeda::media::convertYUV444PToRGBA32(width,height,Y,width,U,width,V,width,dest);
}

void andromeda::media::convertRGBA32Tou255YUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,int linesize_Y,unsigned char* U,int linesize_U,unsigned char* V,int linesize_V)
{
	ColorRGBA back_color(getChannelFloat(br),getChannelFloat(bg),getChannelFloat(bb),1.0f);
	Pixel this_pixel;
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			this_pixel=ColorRGBA::mixAlpha(back_color,((Pixel*)data+y*width+x)->toColorRGBA()).toPixel();
			*(Y+y*linesize_Y+x)=0.299*this_pixel.r+0.587*this_pixel.g+0.114*this_pixel.b;
			*(U+y*linesize_U+x)=-0.1687*this_pixel.r-0.3313*this_pixel.g+0.5*this_pixel.b+128;
			*(V+y*linesize_V+x)=0.5*this_pixel.r-0.4187*this_pixel.g-0.0813*this_pixel.b+128;
		}
	}
}

unsigned char** andromeda::media::convertRGBA32Tou255YUV444P(unsigned char* data,int width,int height,float br,float bg,float bb)
{
	unsigned char** bytes_arr=(unsigned char**)malloc(3*sizeof(char*));
	unsigned char* Y=(unsigned char*)malloc(width*height);
	unsigned char* U=(unsigned char*)malloc(width*height);
	unsigned char* V=(unsigned char*)malloc(width*height);
	andromeda::media::convertRGBA32Tou255YUV444P(data,width,height,br,bg,bb,Y,U,V);
	*bytes_arr=Y;
	*(bytes_arr+1)=U;
	*(bytes_arr+2)=V;
	return bytes_arr;
}

void andromeda::media::convertRGBA32Tou255YUV444P(unsigned char* data,int width,int height,float br,float bg,float bb,unsigned char* Y,unsigned char* U,unsigned char* V)
{
	andromeda::media::convertRGBA32Tou255YUV444P(data,width,height,br,bg,bb,Y,width,U,width,V,width);
}

void andromeda::media::convertu255YUV444PToRGBA32(int width,int height,unsigned char* Y,int linesize_Y,unsigned char* U,int linesize_U,unsigned char* V,int linesize_V,unsigned char* dest)
{
	for(int pos_y=0;pos_y<height;++pos_y)
	{
		for(int pos_x=0;pos_x<width;++pos_x)
		{
			unsigned char y=*(Y+pos_y*linesize_Y+pos_x);
			unsigned char u=*(U+pos_y*linesize_U+pos_x);
			unsigned char v=*(V+pos_y*linesize_V+pos_x);
			Pixel& this_pixel=*((Pixel*)dest+pos_y*width+pos_x);
			this_pixel.r=y+1.403*(v-128);
			this_pixel.g=y-0.343*(u-128)-0.714*(v-128);
			this_pixel.b=y+1.77*(u-128);
			this_pixel.a=255;
		}
	}
}

void andromeda::media::convertu255YUV444PToRGBA32(int width,int height,unsigned char* Y,unsigned char* U,unsigned char* V,unsigned char* dest)
{
	andromeda::media::convertu255YUV444PToRGBA32(width,height,Y,width,U,width,V,width,dest);
}

RasterImage& andromeda::media::getRasterImageByu255YUV444P(int width,int height,unsigned char* Y,int linesize_Y,unsigned char* U,int linesize_U,unsigned char* V,int linesize_V)
{
	unsigned char* data=(unsigned char*)malloc(sizeof(Pixel)*width*height);
	convertu255YUV444PToRGBA32(width,height,Y,linesize_Y,U,linesize_U,V,linesize_V,data);
	RasterImage *img=new RasterImage(width,height,(Pixel*)data);
	return *img;
}
