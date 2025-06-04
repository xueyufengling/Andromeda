#ifndef ANDROMEDA_GRAPHICS_COORDTRANSFORM
#define ANDROMEDA_GRAPHICS_COORDTRANSFORM

namespace andromeda {
	namespace graphics {
		class Texture2D;

		typedef void (*AreaCoordTransform)(float,float,float,float,float&,float&,float&,float&,Texture2D*);

		/*GUI坐标系转换为UV坐标系
		 * 若texture为nullptr则传入坐标必须归一化
		 */
		void gui2uv(float input_x1,float input_y1,float input_x2,float input_y2,float& output_x1,float& output_y1,float& output_x2,float& output_y2,Texture2D* texture=nullptr);
	}
}

#endif//ANDROMEDA_GRAPHICS_COORDTRANSFORM
