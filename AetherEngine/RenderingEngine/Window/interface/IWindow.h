#pragma once
//===============================================================================
// desc: Interface for an abstract window, usable by GLFW, Win32, etc.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "WindowContext.h"
#include "EventContext.h"
//===============================================================================


namespace Aether
{
	class Event;
	enum class eRenderAPI;

	class AETHER_API IWindow {
	public:
		virtual ~IWindow() = default;

		virtual AETHER_RESULT Initialize(const WindowContext::WinData& winData, const eRenderAPI currentRenderer) = 0;
		virtual bool WindowShouldClose() = 0;
		virtual void PollEvents() = 0;
		virtual void Terminate() = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		// This will return a native handle (HWND on Windows or a GLFW window handle on Linux)
		virtual void* GetNativeWindowHandle() const = 0;

		// Extra function to get a win32 handle regardless, allowing for GLFW + Direct X on Windows
		virtual void* GetWin32Handle() const = 0;

		inline int GetWidth() { return m_WinData.m_ClientWidth; }
		inline int GetHeight() { return m_WinData.m_ClientHeight; }

		// Accessor for full WinData struct
		const WindowContext::WinData& GetData() const
		{
			return m_WinData;
		}

		// Default copy sett for WinData struct
		void SetData(const WindowContext::WinData& wd)
		{
			m_WinData = wd;
		}

		// Manual setter for WinData struct - not filling it out fully for the time being
		void SetData(int width, int height, std::string name)
		{
			m_WinData.m_ClientWidth = width;
			m_WinData.m_ClientHeight = height;
			m_WinData.m_Title = name;
		}
	protected:
		WindowContext::WinData m_WinData;
	};
};