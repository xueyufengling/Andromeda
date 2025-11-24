#include <andromeda/graphics/sprite.h>

#include <rapidcsv.h>

using namespace andromeda::graphics;
using namespace andromeda::io;

namespace andromeda
{
namespace graphics
{
const char* default_split_info_names[4] = {"xPos", "yPos", "Width", "Height"};
}
}

void equal_division_sprite_sheet::generate_sprites()
{
	splited_area.clear();
	for(int r = 0; r < row; ++r)
		for(int c = 0; c < column; ++c)
			splited_area.add(clip(c, r));
}

compact_sprite_sheet::compact_sprite_sheet(texture2d texture, const char* split_file, char comma, csv_option option, const char** info_names, sprite_split_mode mode, area_coord_transform coord_transform)
{
	splited_area.clear();
	sheet_texture = texture;
	rapidcsv::Document split_info = read_csv(split_file, comma, option);
	size_t num = split_info.GetRowCount();
	for(size_t idx = 0; idx < num; ++idx)
	{
		idx_mapping[split_info.GetRowName(idx)] = idx; //储存字符串到索引的映射
		float x = split_info.GetCell<float>(*info_names, idx);
		float y = split_info.GetCell<float>(*(info_names + 1), idx);
		float width = split_info.GetCell<float>(*(info_names + 2), idx);
		float height = split_info.GetCell<float>(*(info_names + 3), idx);
		uv_coord area;
		coord_transform(x, y, x + width, y + height, area.u1, area.v1, area.u2, area.v2, mode == PIXEL_COORD ? &texture : nullptr);
		splited_area.add(area);
	}
}
