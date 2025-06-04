#ifndef ANDROMEDA_APP_RENDERSYS
#define ANDROMEDA_APP_RENDERSYS

#include <map>

#include "window.h"
#include "../graphics/camera.h"
#include "../util/array_list.h"

namespace andromeda {
	namespace app {
		class RenderSys
		{
		private:
			andromeda::util::ArrayList<andromeda::graphics::Camera> cams; //需要计算渲染的cam

		public:
			RenderSys();

			static int screenWidth;
			static int screenHeight;

			static void initProperties();

			static void getScreenResolution(int* widthPtr,int* heightPtr);
		};
	}
}

#endif//ANDROMEDA_APP_RENDERSYS
