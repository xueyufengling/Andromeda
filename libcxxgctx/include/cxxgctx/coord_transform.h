#ifndef _CXXGCTX_GL_COORDTRANSFORM
#define _CXXGCTX_GL_COORDTRANSFORM

namespace cxxgctx
{
namespace gl
{
class texture2d;
}

typedef void (*area_coord_transform)(float, float, float, float, float&, float&, float&, float&, gl::texture2d*);

/*GUI坐标系转换为UV坐标系
 * 若texture为nullptr则传入坐标必须归一化
 */
void gui2uv(float input_x1, float input_y1, float input_x2, float input_y2, float& output_x1, float& output_y1, float& output_x2, float& output_y2, gl::texture2d* texture = nullptr);

}

#endif//_CXXGCTX_GL_COORDTRANSFORM
