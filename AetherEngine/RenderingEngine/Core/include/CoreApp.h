#pragma once
//===============================================================================
// desc: The core rendering app that facilitates DLL exporting for use in the main AetherApp
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "AetherUtils.h"
#include "IWindow.h"
#include "Event.h"
#include "IRenderer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API Application
	{
	public:
		Application();
		virtual ~Application() {}

		// This is where the main game loop will live
		AETHER_RESULT Run();

		// Event callback
		void OnEvent(Event& event);

	private:
		std::unique_ptr<IWindow> m_Window;
		std::unique_ptr<IRenderer> m_Renderer;
	};

	// External function to be defined in client applications (games)
	Application* CreateApplication();

};