//===============================================================================
// desc: The core input polling class that handles all inputs, API agnostic
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Input.h"
#include "CoreApp.h"	// For singleton to grab window
//===============================================================================

bool Aether::Input::IsKeyPressed(KeyCode key)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindowHandle());
	auto state = glfwGetKey(window, static_cast<int32_t>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}
