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
	class WindowGLFW : public IWindow
	{
	public:
		WindowGLFW(const WinData& winData);
		virtual ~WindowGLFW();

		AETHER_RESULT Initialize(const WinData& winData) override;
		bool WindowShouldClose() override;
		void PollEvents() override
		{
			glfwPollEvents();
			glfwSwapBuffers(m_pWindow);
		}

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_WinData.m_EventCallback = callback; }

		void Terminate() override;

		HWND GetWin32GLFWHandle() { return (glfwGetWin32Window(m_pWindow)); }

		void* GetNativeWindowHandle() const override
		{
			return m_pWindow; // Handle other platforms
		}

	private:
		GLFWwindow* m_pWindow;

		//// Extension of WinData to handle this specific windows callback functions! Putting the callback function in the base win data struct breaks the std::bind function we use
		//struct WinCallbackData
		//{
		//	WinData m_BaseWinData = {};
		//	EventCallbackFn m_EventCallback;
		//};

		//WinCallbackData m_GLFWData;
	};
};