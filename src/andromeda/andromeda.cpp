#include <opengl/glad/glad.h>
#include <opengl/GLFW/glfw3.h>
#include <openal/alc.h>
#include <openal/al.h>
#include <andromeda/util/log.h>
#include <andromeda/app/render_sys.h>

namespace andromeda {
	bool use_opengl=true;
	bool use_openal=true;

	static bool loaded_opengl=false;

	static void _glfw_error_print(int err_code,const char* description)
	{
		LOG_DEBUG("GLFW Error Code:",err_code,"\nDescription:",description)
	}

	void _glfw_framebuffer_size_callback(GLFWwindow* window,int width,int height)
	{
		glViewport(0,0,width,height);
	}

	namespace audio {
		ALCdevice* al_device=nullptr;
		ALCcontext* al_context=nullptr;
		ALboolean al_support_extension;
	}

	bool _load_opengl()
	{
		if(loaded_opengl)
			return true;
		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LOG_DEBUG("Initialize GLAD failed.");
			return false;
		}
		loaded_opengl=true;
		return true;
	}

	static void _load_glfw()
	{
		if(!glfwInit())
			LOG_DEBUG("Initialize GLFW failed.")
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
		glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		glfwSetErrorCallback(andromeda::_glfw_error_print);
		andromeda::app::RenderSys::initProperties();
	}

	static void _term_glfw()
	{
		glfwTerminate();
	}

	static void _load_openal()
	{
		audio::al_device=alcOpenDevice(NULL);
		if(audio::al_device)
		{
			audio::al_context=alcCreateContext(audio::al_device,NULL);
			alcMakeContextCurrent(audio::al_context);
		}
		audio::al_support_extension=alIsExtensionPresent("EAX2.0");
		alGetError(); //清除错误缓冲区
	}

	static void _term_openal()
	{
		audio::al_context=alcGetCurrentContext();
		audio::al_device=alcGetContextsDevice(audio::al_context);
		alcMakeContextCurrent (NULL);
		alcDestroyContext(audio::al_context);
		alcCloseDevice(audio::al_device);
	}
}

using namespace andromeda;

static __attribute__((constructor)) void _init_lib()
{
	if(use_opengl)
		_load_glfw();
	if(use_openal)
		_load_openal();
}

static __attribute__((destructor)) void _term_lib()
{
	if(use_opengl)
		_term_glfw();
	if(use_openal)
		_term_openal();
}

