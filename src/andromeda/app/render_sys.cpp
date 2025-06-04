#include <andromeda/app/render_sys.h>

using namespace andromeda::app;

 int RenderSys::screenWidth=0;
 int RenderSys::screenHeight=0;

RenderSys::RenderSys() :
		cams(1)
{

}

void RenderSys::initProperties()
{
	getScreenResolution((int*)(&screenWidth),(int*)(&screenHeight));
}

void RenderSys::getScreenResolution(int* widthPtr,int* heightPtr)
{
	const GLFWvidmode* mode=glfwGetVideoMode(glfwGetPrimaryMonitor());
	if(mode)
	{
		*widthPtr=mode->width;
		*heightPtr=mode->height;
	}
}
