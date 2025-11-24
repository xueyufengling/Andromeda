#ifndef ANDROMEDA_GRAPHICS_SPRITE
#define ANDROMEDA_GRAPHICS_SPRITE

#include <andromeda/util/array.h>
#include <map>
#include "texture_2d.h"
#include "coord_transform.h"
#include "geometry.h"
#include "../io/files.h"
#include "../util/timer.h"

namespace andromeda
{
namespace graphics
{
//精灵表，该类的任何派生类都需要在andromeda::graphics命名空间下，因为类型推断traits类__deduce_type在该命名空间下
template<typename Derived>
class sprite_sheet
{
protected:
	texture2d sheet_texture;
	andromeda::util::array<uv_coord> splited_area;
	private:
	sprite_sheet() = default;

	sprite_sheet(texture2d texture, andromeda::util::array<uv_coord> splited_area) :
			sheet_texture(texture), splited_area(splited_area)
	{

	}

	friend Derived; //防止用户CRTP继承时传错派生类模板参数，且该类的子类不能再派生子类，限制了继承链只有该父类及其直接子类

public:
	__attribute__((always_inline)) inline operator GLuint()
	{
		return sheet_texture;
	}

	__attribute__((always_inline)) inline operator texture2d()
	{
		return sheet_texture;
	}

	template<typename KeyType>
	inline uv_coord get_sprite(KeyType key)
	{
		return (Derived*)this->get_sprite(key);
	}

	//GUI坐标系下的裁剪
	inline uv_coord clip_in_gui_coord(int x, int y, int width, int height)
	{
		uv_coord area;
		gui2uv(x, y, x + width, y + height, area.u1, area.v1, area.u2, area.v2, &sheet_texture);
		return area;
	}

	inline uv_coord clip_in_normalized_gui_coord(float x, float y, float width, float height)
	{
		uv_coord area;
		gui2uv(x, y, x + width, y + height, area.u1, area.v1, area.u2, area.v2, nullptr);
		return area;
	}

	inline uv_coord clip_in_uv_coord(float x, float y, float width, float height)
	{
		return uv_coord{x, y, x + width, y + height};
	}

	inline uv_coord get_sprite(int key)
	{
		return splited_area[key];
	}
};

#include "../traits/deduction.h"
#define DefineSpriteSheet(key_type,sprite_sheet_type) set_mapping_type(key_type,sprite_sheet_type)

//用于映射getSprite<int>()
class _sprite_sheet: public sprite_sheet<_sprite_sheet>
{
public:
	using sprite_sheet<_sprite_sheet>::get_sprite;
};
DefineSpriteSheet(int, _sprite_sheet)

//裁剪索引，左上角为原点，向右向下为正
using sprite_sheet_divisor_index=andromeda::math::vector<2,int>;

//等分裁剪，每个裁剪区域相同大小
class equal_division_sprite_sheet: public sprite_sheet<equal_division_sprite_sheet>
{
private:
	using sprite_sheet<equal_division_sprite_sheet>::sheet_texture;
	using sprite_sheet<equal_division_sprite_sheet>::splited_area;
	//多少行多少列
	int column, row;
	//float均为归一化坐标，裁剪区域在精灵表中的坐标，左上角为原点，向右向下为正
	float clip_stride_x, clip_stride_y; //裁剪步长，两个相邻精灵图像之间的UV坐标差
	float clip_offset_x, clip_offset_y; //有效区域左上角顶点位移
	float clip_end_offset_x, clip_end_offset_y; //有效区域右下角顶点位移

	float sprite_width, sprite_height; //裁剪单个精灵图像的UV坐标宽高

	__attribute__((always_inline)) inline void clacl_sprite_size()
	{
		sprite_width = (clip_end_offset_x - clip_offset_x - clip_stride_x * (column - 1)) / column;
		sprite_height = (clip_end_offset_y - clip_offset_y - clip_stride_y * (row - 1)) / row;
	}

	void generate_sprites();

public:
	equal_division_sprite_sheet(texture2d texture, int column = 1, int row = 1, float clip_stride_x = 0, float clip_stride_y = 0, float clip_offset_x = 0, float clip_offset_y = 0, float clip_end_offset_x = 1, float clip_end_offset_y = 1) :
			column(column), row(row), clip_stride_x(clip_stride_x), clip_stride_y(clip_stride_y), clip_offset_x(clip_offset_x), clip_offset_y(clip_offset_y), clip_end_offset_x(clip_end_offset_x), clip_end_offset_y(clip_end_offset_y)
	{
		sheet_texture = texture;
		clacl_sprite_size();
		generate_sprites();
	}

	equal_division_sprite_sheet(texture2d texture, int column = 1, int row = 1, int clip_stride_x = 0, int clip_stride_y = 0, float clip_offset_x = 0, float clip_offset_y = 0, float clip_end_offset_x = 1, float clip_end_offset_y = 1) :
			column(column), row(row), clip_stride_x((float)clip_stride_x / texture.get_width()), clip_stride_y((float)clip_stride_y / texture.get_height()), clip_offset_x(clip_offset_x), clip_offset_y(clip_offset_y), clip_end_offset_x(clip_end_offset_x), clip_end_offset_y(clip_end_offset_y)
	{
		sheet_texture = texture;
		clacl_sprite_size();
		generate_sprites();
	}

	equal_division_sprite_sheet(texture2d texture, int column = 1, int row = 1, int clip_stride_x = 0, int clip_stride_y = 0, int clip_offset_x = 0, int clip_offset_y = 0, int clip_end_offset_x = 1, int clip_end_offset_y = 1) :
			column(column), row(row), clip_stride_x((float)clip_stride_x / texture.get_width()), clip_stride_y((float)clip_stride_y / texture.get_height()), clip_offset_x((float)clip_offset_x / texture.get_width()), clip_offset_y((float)clip_offset_y / texture.get_height()), clip_end_offset_x((float)clip_end_offset_x / texture.get_width()), clip_end_offset_y((float)clip_end_offset_y / texture.get_height())
	{
		sheet_texture = texture;
		clacl_sprite_size();
		generate_sprites();
	}

	inline uv_coord clip(sprite_sheet_divisor_index xy_index)
	{
		return clip(xy_index[0], xy_index[1]);
	}

	inline uv_coord clip(int x, int y)
	{
		return uv_coord{clip_offset_x + (sprite_width + clip_stride_x) * x, 1 - (clip_offset_y + (sprite_height + clip_stride_y) * y + sprite_height), clip_offset_x + (sprite_width + clip_stride_x) * x + sprite_width, 1 - (clip_offset_y + (sprite_height + clip_stride_y) * y)};
	}

	inline uv_coord get_sprite(sprite_sheet_divisor_index xy_index)
	{
		return splited_area[xy_index[1] * column + xy_index[0]];
	}
};
DefineSpriteSheet(sprite_sheet_divisor_index, equal_division_sprite_sheet)

//指定裁剪区域是采用UV坐标裁剪还是像素坐标裁剪
enum sprite_split_mode
{
	UV_COORD, PIXEL_COORD
};

extern const char* default_split_info_names[4];

//紧凑的精灵表，裁剪区域大小、位置不固定，需要额外的CSV文件指定每个裁剪区域的位置和尺寸
class compact_sprite_sheet: public sprite_sheet<compact_sprite_sheet>
{
private:
	using sprite_sheet<compact_sprite_sheet>::sheet_texture;
	using sprite_sheet<compact_sprite_sheet>::splited_area;
	std::map<std::string, int> idx_mapping;

public:
	//info_names依次储存了CSV中切割精灵的左上角x、左上角y、宽度、高度列的名字，mode指定坐标是UV坐标（原点在左下角）还是像素坐标（原点在左上角）
	compact_sprite_sheet(texture2d texture, const char* split_file, char comma = ',', andromeda::io::csv_option option = andromeda::io::csv_option::BOTH_HEADER, const char** info_names = default_split_info_names, sprite_split_mode mode = PIXEL_COORD, area_coord_transform coord_transform = &gui2uv);

	inline uv_coord get_sprite(std::string split_key)
	{
		return splited_area[idx_mapping[split_key]];
	}
};
DefineSpriteSheet(std::string, compact_sprite_sheet)

class sprite: public polygon
{
public:
	typedef struct part
	{
		int sprite_sheet_idx = 0; //该部分对应的精灵表索引
		uv_coord uv_coords;

		part() = default;
		part(int sprite_sheet) :
				sprite_sheet_idx(sprite_sheet)
		{

		}

		inline part& set_uv(float u1, float v1, float u2, float v2)
		{
			uv_coords.u1 = u1;
			uv_coords.v1 = v1;
			uv_coords.u2 = u2;
			uv_coords.v2 = v2;
			return *this;
		}

		inline part& set_uv(uv_coord uv_coords)
		{
			this->uv_coords = uv_coords;
			return *this;
		}

		template<typename KeyType>
		inline part& set_uv(void* sprite_sheet, KeyType idx)
		{
			this->uv_coords = ((typename __mapping_type<KeyType>::result_type*)sprite_sheet)->getSprite(idx);
			return *this;
		}
	} part;

	typedef struct frame
	{
		andromeda::util::array<part> parts;
		float time = 0; //持续时间

		frame() = default;
		frame(float t) :
				time(t)
		{

		}

		inline operator andromeda::util::array<part>&()
		{
			return parts;
		}

		inline operator part*()
		{
			return (part*)parts;
		}

		inline part& operator[](size_t idx)
		{
			return parts[idx];
		}
	} frame;

	template<typename IndexType>
	class sprite_sheet_index
	{
	public:
		IndexType splited_area_idx;
		int sprite_sheet_idx = 0; //该部分对应的精灵表索引
	};

private:
	andromeda::util::timer timer; //按时间渲染不同的帧

protected:
	andromeda::util::array<void*> sprite_sheet; //所使用到的精灵表数组
	andromeda::util::array<frame> frames; //按顺序播放

public:
	template<typename KeyType>
	inline void append_frame(float time)
	{
	}

	template<typename KeyType>
	inline void append_frame(float time, sprite_sheet_index<KeyType> idx)
	{
		frame frame(time);
		part part(idx.sprite_sheet_idx);
		part.set_uv<KeyType>(sprite_sheet[idx.sprite_sheet_idx], idx.splited_area_idx);
		frame.parts.add(part);
		frames.add(frame);
	}

	template<typename KeyType1, typename ...KeyTypes>
	inline void append_frame(float time, sprite_sheet_index<KeyType1> idx1, sprite_sheet_index<KeyTypes> ...rest_idx)
	{
		append_frame<KeyType1>(time, idx1);
		append_frame(time, rest_idx...);
	}
};
}
}

#endif
