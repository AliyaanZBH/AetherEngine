#pragma once
//===============================================================================
// desc: A manager class with a collection of handy utilies for GLFW windows, useful for both DX and Vulkan
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IWindow.h"
//===============================================================================

// Forward declare for renderer
class IRendererBackend;

// WinMan - Window Manager.
namespace Aether
{
	class WindowGLFW final : public IWindow
	{
	public:
		WindowGLFW(const WinData& winData, const eRenderAPI currentRenderer);
		virtual ~WindowGLFW();

		AETHER_RESULT Initialize(const WinData& winData, const eRenderAPI currentRenderer) override;
		bool WindowShouldClose() override;
		void PollEvents() override
		{
			glfwPollEvents();
		}

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_WinData.m_EventCallback = callback; }

		void Terminate() override;

		void* GetWin32Handle() const override { return (glfwGetWin32Window(m_pWindow)); }

		void* GetNativeWindowHandle() const override
		{
			return m_pWindow; // Handle other platforms
		}

	private:
		GLFWwindow* m_pWindow;
	};
};