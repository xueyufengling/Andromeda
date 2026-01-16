#include <cxxactx/al_basic.h>

using namespace cxxactx;

bool al_context::open_device(const char* dev_name)
{
	if(!(al_device = alc_OpenDevice(dev_name)))
	{
		LogError("open audio device ", dev_name, " failed");
		goto FAILED;
	}
	if(!(obj_id = alc_CreateContext(al_device, nullptr)))
	{
		LogError("create audio context for ", dev_name, " failed");
		goto FAILED;
	}
	return true;
	FAILED:
	close_device();
	return false;
}

void al_context::close_device()
{
	alc_DestroyContext(obj_id);
	obj_id = nullptr;
	alc_CloseDevice(al_device);
	al_device = nullptr;
}
