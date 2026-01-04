#pragma once
//===============================================================================
// desc: High-level interface for applications using Aether to query the window.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "WindowContext.h"
//===============================================================================

namespace Aether
{
	class IWindow;

	class AETHER_API Window
	{
		friend class Renderer;
	public:
		static void Initialise(WindowContext::WinData& wd);


		static void Poll();
		static void SetEventCallback(const EventCallbackFn& callback);

		static void SetCursorLocked(const bool lock);

		static bool ShouldClose();
		static int GetWidth();
		static int GetHeight();


		// This will return a native handle (HWND on Windows or a GLFW window handle on Linux)
		static void* GetNativeWindowHandle();
		// Extra function to get a win32 handle regardless, allowing for GLFW + Direct X on Windows
		static void* GetWin32Handle();

	protected:
		// Return the backend interface - ONLY to be used by the high-level Renderer class
		static const IWindow& GetInterface();
	private:
		static std::unique_ptr<IWindow> s_WindowBackend;
	};
}