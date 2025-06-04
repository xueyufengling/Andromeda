#ifndef ANDROMEDA_GRAPHICS_SPRITE
#define ANDROMEDA_GRAPHICS_SPRITE

#include <opengl/glad/glad.h>
#include <map>
#include "texture_2d.h"
#include "coord_transform.h"
#include "geometry.h"
#include "../io/files.h"
#include "../util/array_list.h"
#include "../util/timer.h"

namespace andromeda {
	namespace graphics {
		//精灵表，该类的任何派生类都需要在andromeda::graphics命名空间下，因为类型推断traits类__deduce_type在该命名空间下
		template<typename Derived>
		class SpriteSheet
		{
		protected:
			Texture2D sheet_texture;
			andromeda::util::ArrayList<UvCoord> splited_area;
		private:
			SpriteSheet()=default;

			SpriteSheet(Texture2D texture,andromeda::util::ArrayList<UvCoord> splited_area) :
					sheet_texture(texture), splited_area(splited_area)
			{

			}

			friend Derived; //防止用户CRTP继承时传错派生类模板参数，且该类的子类不能再派生子类，限制了继承链只有该父类及其直接子类

		public:
			__attribute__((always_inline)) inline operator GLuint()
			{
				return sheet_texture.texture_id;
			}

			__attribute__((always_inline)) inline operator Texture2D()
			{
				return sheet_texture;
			}

			template<typename KeyType>
			inline UvCoord getSprite(KeyType key)
			{
				return (Derived*)this->getSprite(key);
			}

			//GUI坐标系下的裁剪
			inline UvCoord clipInGuiCoord(int x,int y,int width,int height)
			{
				UvCoord area;
				gui2uv(x,y,x+width,y+height,area.u1,area.v1,area.u2,area.v2,&sheet_texture);
				return area;
			}

			inline UvCoord clipInNormalizedGuiCoord(float x,float y,float width,float height)
			{
				UvCoord area;
				gui2uv(x,y,x+width,y+height,area.u1,area.v1,area.u2,area.v2,nullptr);
				return area;
			}

			inline UvCoord clipInUvCoord(float x,float y,float width,float height)
			{
				return UvCoord{x,y,x+width,y+height};
			}

			inline UvCoord getSprite(int key)
			{
				return splited_area[key];
			}
		};

#include "../traits/deduction.h"
#define DefineSpriteSheet(key_type,sprite_sheet_type) set_mapping_type(key_type,sprite_sheet_type)

		//用于映射getSprite<int>()
		class _SpriteSheet:public SpriteSheet<_SpriteSheet>
		{
		public:
			using SpriteSheet<_SpriteSheet>::getSprite;
		};
		DefineSpriteSheet(int,_SpriteSheet)

		//裁剪索引，左上角为原点，向右向下为正
		using SpriteSheetDivisorIndex=andromeda::math::Vector<2,int>;

		//等分裁剪，每个裁剪区域相同大小
		class EqualDivisionSpriteSheet:public SpriteSheet<EqualDivisionSpriteSheet>
		{
		private:
			using SpriteSheet<EqualDivisionSpriteSheet>::sheet_texture;
			using SpriteSheet<EqualDivisionSpriteSheet>::splited_area;
			//多少行多少列
			int column,row;
			//float均为归一化坐标，裁剪区域在精灵表中的坐标，左上角为原点，向右向下为正
			float clip_stride_x,clip_stride_y; //裁剪步长，两个相邻精灵图像之间的UV坐标差
			float clip_offset_x,clip_offset_y; //有效区域左上角顶点位移
			float clip_end_offset_x,clip_end_offset_y; //有效区域右下角顶点位移

			float sprite_width,sprite_height; //裁剪单个精灵图像的UV坐标宽高

			__attribute__((always_inline)) inline void claclSpriteSize()
			{
				sprite_width=(clip_end_offset_x-clip_offset_x-clip_stride_x*(column-1))/column;
				sprite_height=(clip_end_offset_y-clip_offset_y-clip_stride_y*(row-1))/row;
			}

			void generateSprites();
		public:
			EqualDivisionSpriteSheet(Texture2D texture,int column=1,int row=1,float clip_stride_x=0,float clip_stride_y=0,float clip_offset_x=0,float clip_offset_y=0,float clip_end_offset_x=1,float clip_end_offset_y=1) :
					column(column), row(row), clip_stride_x(clip_stride_x), clip_stride_y(clip_stride_y), clip_offset_x(clip_offset_x), clip_offset_y(clip_offset_y), clip_end_offset_x(clip_end_offset_x), clip_end_offset_y(clip_end_offset_y)
			{
				sheet_texture=texture;
				claclSpriteSize();
				generateSprites();
			}

			EqualDivisionSpriteSheet(Texture2D texture,int column=1,int row=1,int clip_stride_x=0,int clip_stride_y=0,float clip_offset_x=0,float clip_offset_y=0,float clip_end_offset_x=1,float clip_end_offset_y=1) :
					column(column), row(row), clip_stride_x((float)clip_stride_x/texture.getWidth()), clip_stride_y((float)clip_stride_y/texture.getHeight()), clip_offset_x(clip_offset_x), clip_offset_y(clip_offset_y), clip_end_offset_x(clip_end_offset_x), clip_end_offset_y(clip_end_offset_y)
			{
				sheet_texture=texture;
				claclSpriteSize();
				generateSprites();
			}

			EqualDivisionSpriteSheet(Texture2D texture,int column=1,int row=1,int clip_stride_x=0,int clip_stride_y=0,int clip_offset_x=0,int clip_offset_y=0,int clip_end_offset_x=1,int clip_end_offset_y=1) :
					column(column), row(row), clip_stride_x((float)clip_stride_x/texture.getWidth()), clip_stride_y((float)clip_stride_y/texture.getHeight()), clip_offset_x((float)clip_offset_x/texture.getWidth()), clip_offset_y((float)clip_offset_y/texture.getHeight()), clip_end_offset_x((float)clip_end_offset_x/texture.getWidth()), clip_end_offset_y((float)clip_end_offset_y/texture.getHeight())
			{
				sheet_texture=texture;
				claclSpriteSize();
				generateSprites();
			}

			inline UvCoord clip(SpriteSheetDivisorIndex xy_index)
			{
				return clip(xy_index[0],xy_index[1]);
			}

			inline UvCoord clip(int x,int y)
			{
				return UvCoord{clip_offset_x+(sprite_width+clip_stride_x)*x,1-(clip_offset_y+(sprite_height+clip_stride_y)*y+sprite_height),clip_offset_x+(sprite_width+clip_stride_x)*x+sprite_width,1-(clip_offset_y+(sprite_height+clip_stride_y)*y)};
			}

			inline UvCoord getSprite(SpriteSheetDivisorIndex xy_index)
			{
				return splited_area[xy_index[1]*column+xy_index[0]];
			}
		};
		DefineSpriteSheet(SpriteSheetDivisorIndex,EqualDivisionSpriteSheet)

		//指定裁剪区域是采用UV坐标裁剪还是像素坐标裁剪
		enum SpriteSplitMode
		{
			UV_COORD, PIXEL_COORD
		};

		extern const char* default_split_info_names[4];

		//紧凑的精灵表，裁剪区域大小、位置不固定，需要额外的CSV文件指定每个裁剪区域的位置和尺寸
		class CompactSpriteSheet:public SpriteSheet<CompactSpriteSheet>
		{
		private:
			using SpriteSheet<CompactSpriteSheet>::sheet_texture;
			using SpriteSheet<CompactSpriteSheet>::splited_area;
			std::map<std::string,int> idx_mapping;

		public:
			//info_names依次储存了CSV中切割精灵的左上角x、左上角y、宽度、高度列的名字，mode指定坐标是UV坐标（原点在左下角）还是像素坐标（原点在左上角）
			CompactSpriteSheet(Texture2D texture,const char* split_file,char comma=',',andromeda::io::CsvOption option=andromeda::io::CsvOption::BOTH_HEADER,const char** info_names=default_split_info_names,SpriteSplitMode mode=PIXEL_COORD,AreaCoordTransform coord_transform=&gui2uv);

			inline UvCoord getSprite(std::string split_key)
			{
				return splited_area[idx_mapping[split_key]];
			}
		};
		DefineSpriteSheet(std::string,CompactSpriteSheet)

		class Sprite:public Polygon
		{
		public:
			typedef struct Part
			{
				int sprite_sheet_idx=0; //该部分对应的精灵表索引
				UvCoord uv_coords;

				Part()=default;
				Part(int sprite_sheet) :
						sprite_sheet_idx(sprite_sheet)
				{

				}

				inline Part& setUv(float u1,float v1,float u2,float v2)
				{
					uv_coords.u1=u1;
					uv_coords.v1=v1;
					uv_coords.u2=u2;
					uv_coords.v2=v2;
					return *this;
				}

				inline Part& setUv(UvCoord uv_coords)
				{
					this->uv_coords=uv_coords;
					return *this;
				}

				template<typename KeyType>
				inline Part& setUv(void* sprite_sheet,KeyType idx)
				{
					this->uv_coords=((typename __mapping_type<KeyType>::result_type*)sprite_sheet)->getSprite(idx);
					return *this;
				}
			} FramePart;

			typedef struct Frame
			{
				andromeda::util::ArrayList<Part> parts;
				float time=0; //持续时间

				Frame()=default;
				Frame(float t) :
						time(t)
				{

				}

				inline operator andromeda::util::ArrayList<Part>&()
				{
					return parts;
				}

				inline operator Part*()
				{
					return parts.elements();
				}

				inline Part& operator[](size_t idx)
				{
					return parts[idx];
				}
			} Frame;

			template<typename IndexType>
			class SpriteSheetIndex
			{
			public:
				IndexType splited_area_idx;
				int sprite_sheet_idx=0; //该部分对应的精灵表索引
			};

		private:
			andromeda::util::Timer timer; //按时间渲染不同的帧

		protected:
			andromeda::util::ArrayList<void*> sprite_sheet; //所使用到的精灵表数组
			andromeda::util::ArrayList<Frame> frames; //按顺序播放

		public:
			template<typename KeyType>
			inline void appendFrame(float time)
			{
			}

			template<typename KeyType>
			inline void appendFrame(float time,SpriteSheetIndex<KeyType>idx)
			{
				Frame frame(time);
				Part part(idx.sprite_sheet_idx);
				part.setUv<KeyType>(sprite_sheet[idx.sprite_sheet_idx],idx.splited_area_idx);
				frame.parts.add(part);
				frames.add(frame);
			}

			template<typename KeyType1,typename ...KeyTypes>
			inline void appendFrame(float time,SpriteSheetIndex<KeyType1>idx1,SpriteSheetIndex<KeyTypes>...rest_idx)
			{
				appendFrame<KeyType1>(time,idx1);
				appendFrame(time,rest_idx...);
			}
		};
	}
}

#endif
