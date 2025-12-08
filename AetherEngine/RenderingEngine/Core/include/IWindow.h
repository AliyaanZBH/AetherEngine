#pragma once
//===============================================================================
// desc: Interface for an abstract window, usable by GLFW, Win32, etc.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

class Event;

class AETHER_API IWindow {
public:
	// Nifty struct to access window data from the main process. Public so that the renderer can see this too
	struct WinData
	{
		// All windows apps have these handles
	/*	HINSTANCE hAppInst = 0;
		HWND      hMainWnd = 0;
		HICON     hIcon = 0;*/
		std::string m_MainWndCaption = "Aether Engine";
		int m_ClientWidth;
		int m_ClientHeight;
		bool m_bAppPaused = false;
		bool m_bMinimized = false;
		bool m_bMaximized = false;
		bool m_bResizing = false;
		bool m_bFullscreen = false;
	};
protected:
	WinData m_WinData;
public:
	using EventCallbackFn = std::function<void(Event&)>;

    virtual ~IWindow() = default;

    virtual bool Initialize(const WinData& winData) = 0;
	virtual bool WindowShouldClose() = 0;
	virtual void PollEvents() = 0;

	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

	// This will return a native handle (e.g., HWND for Win32 or a window handle for GLFW)
	virtual void* GetNativeWindowHandle() const = 0;

	// Accessor for WinData struct
	const WinData& GetData() const
	{
		return m_WinData;
	}

	// Setter for WinData struct - not filling it out fully for the time being
	void SetData(int width, int height, std::string name)
	{
		m_WinData.m_ClientWidth = width;
		m_WinData.m_ClientHeight = height;
		m_WinData.m_MainWndCaption = name;
	}
};