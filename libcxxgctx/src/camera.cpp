#include <cxxgctx/camera.h>

#include <cxxsci/transform.h>

using namespace cxxgctx;
using namespace cxxsci;

camera::camera(float znear, float zfar, float fov, float aspect_ratio) :
		frustum_near(znear), frustum_far(zfar), fov(fov), aspect_ratio(aspect_ratio)
{
	projection = cxxsci::frustum(znear, zfar, fov, aspect_ratio);
}

matrix4x4f camera::calc_view_projection_matrix()
{
	matrix4x4f pv_mat = shift(-cam_position);
	cam_view = {cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * sin(yaw)};
	pv_mat = pv_mat * Rzyx(-pitch, -yaw, -roll);
	pv_mat = pv_mat * projection;
	return pv_mat;
}
