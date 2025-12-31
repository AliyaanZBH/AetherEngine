//===============================================================================
// desc: High-level interface for applications using Aether to query the window.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Window.h"
#include "IWindow.h"
#include "GraphicsContext.h"

#ifdef USE_GLFW
#include "WindowGLFW.h"
#endif
//===============================================================================

namespace Aether
{
	std::unique_ptr<IWindow> Window::s_WindowBackend = nullptr;

	void Window::Initialise(WindowContext::WinData& wd)
	{

    #ifdef USE_GLFW
		s_WindowBackend = std::make_unique<WindowGLFW>(wd, GraphicsContext::GetRenderAPI());      // Calls initialise and catches errors inside with assert
	#elif defined(USE_WIN32)
		s_WindowBackend = std::make_unique<WinManWin32>();
    #else
    #error No window API defined. Please enable USE_GLFW or USE_WIN32."
        ar = AETHER_FAIL;
    #endif
	}

	void* Window::GetNativeWindowHandle()
	{
		return s_WindowBackend->GetNativeWindowHandle();
	}
	void* Window::GetWin32Handle()
	{
		return s_WindowBackend->GetWin32Handle();
	}
	const IWindow& Window::GetInterface()
	{
		return *s_WindowBackend;
	}

	void Window::Poll()
	{
		s_WindowBackend->PollEvents();
	}

	void Window::SetEventCallback(const std::function<void(Event&)>& callback)
	{
		s_WindowBackend->SetEventCallback(callback);
	}


	inline bool Window::ShouldClose()
	{
		return s_WindowBackend->WindowShouldClose();
	}

	inline int Window::GetWidth()
	{ 
		return s_WindowBackend->GetWidth(); 
	}

	inline int Window::GetHeight()
	{ 
		return s_WindowBackend->GetHeight();
	}

};