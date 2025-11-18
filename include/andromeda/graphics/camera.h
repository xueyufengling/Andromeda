#ifndef ANDROMEDA_GRAPHICS_CAMERA
#define ANDROMEDA_GRAPHICS_CAMERA

#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/transform.h"
#include "framebuffer.h"
#include "../app/window.h"

namespace andromeda
{
namespace graphics
{
class camera
{
private:
	andromeda::math::vector3f cam_view; //视角正前方
	andromeda::math::matrix4x4f projection; //投影矩阵，每次更新near,far,fov,aspect之后均需要更新
	float frustum_near, frustum_far, fov, aspect_ratio;

protected:
	void process_input();

public:
	andromeda::math::vector3f cam_position; //位置
	float yaw = 0, pitch = 0, roll = 0;
	framebuffer* bind_framebuffer = nullptr;

	camera(float znear = 0.1, float zfar = 100, float fov = 1.0f, float aspect_ratio = 1920 / 1080, andromeda::graphics::framebuffer* buffer = nullptr);

	andromeda::math::matrix4x4f calc_view_projection_matrix();

	inline andromeda::math::vector3f get_cam_direction()
	{
		return cam_view;
	}

	inline camera& set_z_near(float znear)
	{
		frustum_near = znear;
		projection = andromeda::math::frustum(znear, frustum_far, fov, aspect_ratio);
		return *this;
	}

	inline camera& set_z_far(float zfar)
	{
		frustum_far = zfar;
		projection = andromeda::math::frustum(frustum_near, zfar, fov, aspect_ratio);
		return *this;
	}

	inline camera& set_fov(float fov)
	{
		this->fov = fov;
		projection = andromeda::math::frustum(frustum_near, frustum_far, fov, aspect_ratio);
		return *this;
	}

	inline camera& set_aspect_ratio(float aspect_ratio)
	{
		this->aspect_ratio = aspect_ratio;
		projection = andromeda::math::frustum(frustum_near, frustum_far, fov, aspect_ratio);
		return *this;
	}

	inline camera& set_aspect_ratio(andromeda::app::window* window)
	{
		this->aspect_ratio = (float)(window->get_height()) / window->get_width();
		projection = andromeda::math::frustum(frustum_near, frustum_far, fov, aspect_ratio);
		return *this;
	}

	inline float get_z_near()
	{
		return frustum_near;
	}

	inline float get_z_far()
	{
		return frustum_far;
	}

	inline float get_fov()
	{
		return fov;
	}

	inline float get_aspect_ratio()
	{
		return aspect_ratio;
	}
};
}
}

#endif//ANDROMEDA_GRAPHICS_CAMERA
