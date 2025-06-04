#include <andromeda/graphics/camera.h>

#include <andromeda/math/transform.h>

#ifdef far
#undef far
#define __SHADOWED_DEF_MARK_far
#endif

#ifdef near
#undef near
#define __SHADOWED_DEF_MARK_near
#endif

using namespace andromeda::graphics;
using namespace andromeda::math;

Camera::Camera(float znear,float zfar,float fov,float aspect_ratio,Framebuffer *buffer) :
		near(znear), far(zfar), fov(fov), aspect_ratio(aspect_ratio), framebuffer(buffer)
{
	projection=andromeda::math::frustum(znear,zfar,fov,aspect_ratio);
}

Matrix4x4f Camera::calcViewProjectionMatrix()
{
	Matrix4x4f pv_mat=shift(-cam_position);
	cam_view={cos(pitch)*cos(yaw),sin(pitch),cos(pitch)*sin(yaw)};
	pv_mat=pv_mat*Rzyx(-pitch,-yaw,-roll);
	pv_mat=pv_mat*projection;
	return pv_mat;
}

#ifdef __SHADOWED_DEF_MARK_far
#define far
#undef __SHADOWED_DEF_MARK_far
#endif

#ifdef __SHADOWED_DEF_MARK_near
#define near
#undef __SHADOWED_DEF_MARK_near
#endif
