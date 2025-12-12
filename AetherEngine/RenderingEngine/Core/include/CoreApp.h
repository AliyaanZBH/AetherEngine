#pragma once
//===============================================================================
// desc: The core engine app that facilitates DLL exporting and general use for any application made with Aether!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "AetherUtils.h"
#include "IWindow.h"
#include "IRenderer.h"
#include "Event.h"
#include "LayerStack.h"
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

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	private:
		std::unique_ptr<IWindow> m_Window;
		std::unique_ptr<IRenderer> m_Renderer;

		LayerStack m_LayerStack;
	};

	// External function to be defined in client applications (games)
	Application* CreateApplication();

};