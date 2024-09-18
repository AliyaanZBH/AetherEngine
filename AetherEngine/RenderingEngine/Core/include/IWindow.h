#pragma once
//===============================================================================
// desc: Interface for an abstract window, usable by GLFW, Win32, etc.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <string>
//===============================================================================

class IWindow {
protected:
	// Nifty struck to access window data from the main process.
	struct WinData
	{
		// All windows apps have these handles
	/*	HINSTANCE hAppInst = 0;
		HWND      hMainWnd = 0;
		HICON     hIcon = 0;*/
		std::string m_MainWndCaption;
		int m_ClientWidth;
		int m_ClientHeight;
		bool m_AppPaused = false;
		bool m_Minimized = false;
		bool m_Maximized = false;
		bool m_Resizing = false;
	};

	WinData m_WinData;

	//IRenderer* m_Renderer;

	//IWindow()
	//	: m_Renderer(nullptr)
	//{}
public:
    virtual ~IWindow() = default;

    virtual bool Initialize(const WinData& winData) = 0;
	virtual bool WindowShouldClose() = 0;
	virtual void PollEvents() = 0;

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