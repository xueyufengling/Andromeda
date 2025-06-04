#ifndef ANDROMEDA_GRAPHICS_CAMERA
#define ANDROMEDA_GRAPHICS_CAMERA

#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/transform.h"
#include "framebuffer.h"
#include "../app/window.h"

//变量名与minwindef.h中的宏冲突
#ifdef far
#undef far
#define __SHADOWED_DEF_MARK_far
#endif

#ifdef near
#undef near
#define __SHADOWED_DEF_MARK_near
#endif

namespace andromeda {
	namespace graphics {
		class Camera
		{
		private:
			andromeda::math::Vector3f cam_view; //视角正前方
			andromeda::math::Matrix4x4f projection; //投影矩阵，每次更新near,far,fov,aspect之后均需要更新
			float near,far,fov,aspect_ratio;

		protected:
			void processInput();

		public:
			andromeda::math::Vector3f cam_position; //位置
			float yaw=0,pitch=0,roll=0;
			Framebuffer* framebuffer=nullptr;

			Camera(float znear=0.1,float zfar=100,float fov=1.0f,float aspect_ratio=1920/1080,Framebuffer* buffer=nullptr);

			andromeda::math::Matrix4x4f calcViewProjectionMatrix();

			inline andromeda::math::Vector3f getCamDirection()
			{
				return cam_view;
			}

			inline Camera& setZNear(float znear)
			{
				near=znear;
				projection=andromeda::math::frustum(znear,far,fov,aspect_ratio);
				return *this;
			}

			inline Camera& setZFar(float zfar)
			{
				far=zfar;
				projection=andromeda::math::frustum(near,zfar,fov,aspect_ratio);
				return *this;
			}

			inline Camera& setFOV(float fov)
			{
				this->fov=fov;
				projection=andromeda::math::frustum(near,far,fov,aspect_ratio);
				return *this;
			}

			inline Camera& setAspectRatio(float aspect_ratio)
			{
				this->aspect_ratio=aspect_ratio;
				projection=andromeda::math::frustum(near,far,fov,aspect_ratio);
				return *this;
			}

			inline Camera& setAspectRatio(andromeda::app::Window* window)
			{
				this->aspect_ratio=(float)(window->getHeight())/window->getWidth();
				projection=andromeda::math::frustum(near,far,fov,aspect_ratio);
				return *this;
			}

			inline float getZNear()
			{
				return near;
			}

			inline float getZFar()
			{
				return far;
			}

			inline float getFOV()
			{
				return fov;
			}

			inline float getAspectRatio()
			{
				return aspect_ratio;
			}
		};
	}
}

#ifdef __SHADOWED_DEF_MARK_far
#define far
#undef __SHADOWED_DEF_MARK_far
#endif

#ifdef __SHADOWED_DEF_MARK_near
#define near
#undef __SHADOWED_DEF_MARK_near
#endif

#endif//ANDROMEDA_GRAPHICS_CAMERA
