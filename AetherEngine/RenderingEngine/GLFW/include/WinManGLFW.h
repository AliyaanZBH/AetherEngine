#pragma once
//===============================================================================
// desc: A manager class with a collection of handy utilies for GLFW windows, useful for both DX and Vulkan
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#include "IWindow.h"
//===============================================================================

// Forward declare for renderer
class IRenderer;


// WinMan meaning Window Manager.
class WinManGLFW : public IWindow
{
public:
	bool Initialize(const WinData& winData) override;
	bool WindowShouldClose() override;
	void PollEvents() override
	{
		glfwPollEvents();
	}

	void* GetNativeWindowHandle() const override
	{
		#ifdef _WIN32
				return static_cast<void*>(glfwGetWin32Window(m_pWindow));
		#else
				return nullptr;  // Handle other platforms
		#endif
	}
private:
	GLFWwindow* m_pWindow;
};