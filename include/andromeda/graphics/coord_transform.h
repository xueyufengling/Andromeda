#ifndef ANDROMEDA_GRAPHICS_COORDTRANSFORM
#define ANDROMEDA_GRAPHICS_COORDTRANSFORM

namespace andromeda
{
namespace graphics
{
class texture2d;

typedef void (*area_coord_transform)(float, float, float, float, float&, float&, float&, float&, texture2d*);

/*GUI坐标系转换为UV坐标系
 * 若texture为nullptr则传入坐标必须归一化
 */
void gui2uv(float input_x1, float input_y1, float input_x2, float input_y2, float& output_x1, float& output_y1, float& output_x2, float& output_y2, texture2d* texture = nullptr);
}
}

#endif//ANDROMEDA_GRAPHICS_COORDTRANSFORM
