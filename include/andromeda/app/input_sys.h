#ifndef ANDROMEDA_APP_INPUTSYS
#define ANDROMEDA_APP_INPUTSYS

#include <map>

#include "window.h"

namespace andromeda{
	namespace app{
		static void _receive_key_input(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if(action==GLFW_PRESS)
			{
				InputSys::input_map[window]->now_key_states[key]=true;
			}
			else if(action==GLFW_RELEASE)
				InputSys::input_map[window]->now_key_states[key]=false;
		}

		class InputSys
		{
			friend void _receive_key_input(GLFWwindow* window, int key, int scancode, int action, int mods);
		private:
			static std::map<GLFWwindow*,InputSys*> input_map;
			bool past_key_states[GLFW_KEY_LAST];
			bool now_key_states[GLFW_KEY_LAST];
			bool past_mouse_states[GLFW_MOUSE_BUTTON_LAST];
			bool now_mouse_states[GLFW_MOUSE_BUTTON_LAST];

		public:
			InputSys(GLFWwindow* window_)
			{
				glfwSetKeyCallback(window_,_receive_key_input);
				glfwSetKeyCallback(window_,_receive_input);
				input_map[window_]=this;
			}
		};
	}
}

#endif//ANDROMEDA_APP_INPUTSYS
