#pragma once
//===============================================================================
// desc: A small helper class and enum to keep track of the current Window context
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "EventContext.h"
//===============================================================================

namespace Aether
{
	enum class eWindowAPI
	{
		kGLFW,
		kWin32
	};

	class WindowContext {
	public:
		static void SelectWindowAPI(eWindowAPI api) { s_CurrentWindowAPI = api; }
		static eWindowAPI GetWindowAPI() { return s_CurrentWindowAPI; }
	private:
		static inline eWindowAPI s_CurrentWindowAPI = eWindowAPI::kGLFW;
	public:
		// Nifty struct to setup and access window data. Public so that the renderer can see this too
		struct WinData
		{

			// All windows apps have these handles
		/*	HINSTANCE hAppInst = 0;
			HWND      hMainWnd = 0;
			HICON     hIcon = 0;*/

			EventCallbackFn m_EventCallback;

			std::string m_Title = "Aether Engine";
			uint16_t m_ClientWidth = 800u;
			uint16_t m_ClientHeight = 600u;
			bool m_bAppPaused = false;
			bool m_bMinimized = false;
			bool m_bMaximized = false;
			bool m_bResizing = false;
			bool m_bFullscreen = false;
		};
	};
}
