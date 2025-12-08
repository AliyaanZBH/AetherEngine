#pragma once
//===============================================================================
// desc: A manager class with a collection of handy utilies for GLFW windows, useful for both DX and Vulkan
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IWindow.h"
//===============================================================================

// Forward declare for renderer
class IRenderer;

// WinMan - Window Manager.
namespace Aether
{
	class AETHER_API WindowGLFW : public IWindow
	{
	public:
		WindowGLFW() {}
		WindowGLFW(const WinData& winData);
		~WindowGLFW();

		AETHER_RESULT Initialize(const WinData& winData) override;
		bool WindowShouldClose() override;
		void PollEvents() override
		{
			glfwPollEvents();
		}

		void SetEventCallback(const EventCallbackFn& callback) override;

		void Terminate() override;

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
};