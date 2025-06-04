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

#define getChannelRByRadianVector(rdg,gdb,len) ((float)(len*cos(gdb)*cos(rdg)))
#define getChannelGByRadianVector(rdg,gdb,len) ((float)(len*cos(gdb)*sin(rdg)))
#define getChannelBByRadianVector(rdg,gdb,len) ((float)(len*sin(gdb)))
#define getLightChannelInt(i) (i<0.0?0.0:(i<255.0?i/255.0:1.0))
#define getLightChannelFloat(f) (f<0.0?0.0:(f<1.0?f:1.0))
#define getChrominaceChannelInt(i) (i<-127.5?-0.5:(i>127.5?0.5:i/255))
#define getChrominaceChannelFloat(f) (f<-0.5?-0.5:(f>0.5?0.5:f))
#define getPixelLightInt(i) (i<0?0:(i<255?i:255))
#define getPixelLightFloat(f) (f<0?0:(f<1?f*255:255))
#define getPixelChrominaceInt(i) (i<-127.5?-127.5:(i>127.5?127.5:i))
#define getPixelChrominaceFloat(f) (f<-0.5?-0.5:(f>0.5?0.5:f))

namespace andromeda {
	namespace graphics {
		enum ColorChannel
		{
			R=0, G, B, A
		};

		class Pixel;

		class ColorRGBA
		{
		public:
			float r,g,b,a;

			operator Pixel();

			ColorRGBA(int i_r,int i_g,int i_b,int i_a) :
					r(getChannelInt(i_r)), g(getChannelInt(i_g)), b(getChannelInt(i_b)), a(getChannelInt(i_a))
			{

			}

			ColorRGBA(float f_r=0,float f_g=0,float f_b=0,float f_a=0) :
					r(getChannelFloat(f_r)), g(getChannelFloat(f_g)), b(getChannelFloat(f_b)), a(getChannelFloat(f_a))
			{

			}

			ColorRGBA(Pixel pixel);

			inline bool operator==(ColorRGBA destcolor)
			{
				return (r==destcolor.r&&g==destcolor.g&&b==destcolor.b&&a==destcolor.a);
			}

			inline ColorRGBA& setRGBA(ColorRGBA color)
			{
				memcpy(this,&color,sizeof(ColorRGBA));
				return *this;
			}

			inline ColorRGBA& setRGBA(ColorChannel color_ch,float color_value)
			{
				*((float*)this+color_ch)=getChannelFloat(color_value);
				return *this;
			}

			inline float getRGBA(ColorChannel color_ch)
			{
				return *((float*)this+color_ch);
			}

			ColorRGBA& setRGBA(float _r=-1,float _g=-1,float _b=-1,float _a=-1); //设置-1表示不变，0-255则改变为设置的值，超出这个范围的就取0或255

			Pixel toPixel();
			static ColorRGBA mixAlpha(ColorRGBA below,ColorRGBA above);

			inline float grayScale(float weight_r,float weight_g,float weight_b)
			{
				return (r*weight_r+g*weight_g+b*weight_b)/3.0;
			}

			static ColorRGBA WHITE;
			static ColorRGBA BLACK;
			static ColorRGBA RED;
			static ColorRGBA GREEN;
			static ColorRGBA BLUE;
			static ColorRGBA YELLOW;
			static ColorRGBA PURPLE;
			static ColorRGBA CYAN;
			static ColorRGBA TRANSPARENT_BLACK;
		};

		class ColorRGBAVector
		{
		public:
			float rdefg,gdefb,length,alpha;

			ColorRGBAVector(float rdg=0,float gdb=0,float len=0,float a=0);
			ColorRGBAVector(ColorRGBA color);
			ColorRGBAVector(Pixel pixel);

			void operator=(ColorRGBA color);
			ColorRGBA toColorRGBA();
			Pixel toPixel();
		};

		class Pixel
		{
		public:
			unsigned char r,g,b,a;

			operator ColorRGBA()
			{
				return ColorRGBA(r,g,b,a);
			}

			Pixel(unsigned char i_r=0,unsigned char i_g=0,unsigned char i_b=0,unsigned char i_a=0) :
					r(i_r), g(i_g), b(i_b), a(i_a)
			{

			}

			Pixel(float f_r,float f_g,float f_b,float f_a) :
					r(getPixelFloat(f_r)), g(getPixelFloat(f_g)), b(getPixelFloat(f_b)), a(getPixelFloat(f_a))
			{

			}

			Pixel(ColorRGBA color) :
					r(getPixelFloat(color.r)), g(getPixelFloat(color.g)), b(getPixelFloat(color.b)), a(getPixelFloat(color.a))
			{

			}

			inline bool operator==(Pixel destpixel)
			{
				return (r==destpixel.r&&g==destpixel.g&&b==destpixel.b&&a==destpixel.a);
			}

			ColorRGBA toColorRGBA()
			{
				return ColorRGBA(r,g,b,a);
			}

			inline uint32_t packToInt()
			{
				return (((((((uint32_t)r)<<8)|g)<<8)|b)<<8)|a;
			}

			inline Pixel& setRGBA(Pixel pixel)
			{
				memcpy(this,&pixel,sizeof(Pixel));
				return *this;
			}

			inline Pixel& setRGBA(ColorChannel color_ch,float color_value)
			{
				*((int*)this+color_ch)=getChannelInt(color_value);
				return *this;
			}

			inline int getRGBA(ColorChannel color_ch)
			{
				return *((int*)this+color_ch);
			}

			Pixel& setRGBA(int _r=-1,int _g=-1,int _b=-1,int _a=-1); //设置-1表示不变，0-255则改变为设置的值，超出这个范围的就取0或255

			inline Pixel& mulFactor(float f)
			{
				r*=f;
				g*=f;
				b*=f;
				return *this;
			}

			static Pixel WHITE;
			static Pixel BLACK;
			static Pixel RED;
			static Pixel GREEN;
			static Pixel BLUE;
			static Pixel YELLOW;
			static Pixel PURPLE;
			static Pixel CYAN;
			static Pixel TRANSPARENT_BLACK;
		};
	}
}

#endif //ANDROMEDA_GRAPHICS_COLORRGBA
