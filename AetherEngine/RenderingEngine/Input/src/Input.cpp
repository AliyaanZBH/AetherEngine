//===============================================================================
// desc: The core input polling class that handles all inputs, API agnostic
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Input.h"
#include "CoreApp.h"	// For singleton to grab window
#include "Window.h"
//===============================================================================
namespace Aether
{
	bool Input::IsKeyPressed(KeyCode key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Window::GetNativeWindowHandle());
		int state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	//bool Input::IsKeyReleased(KeyCode key)
	//{
	//	GLFWwindow* window = static_cast<GLFWwindow*>(Window::GetNativeWindowHandle());
	//	int state = glfwGetKey(window, static_cast<int32_t>(key));
	//	return state == GLFW_RELEASE;
	//}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Window::GetNativeWindowHandle());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Window::GetNativeWindowHandle());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
}
