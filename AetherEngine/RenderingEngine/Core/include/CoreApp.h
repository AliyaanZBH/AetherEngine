#pragma once
//===============================================================================
// desc: The core engine app that facilitates general use for any application made with Aether!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "AetherUtils.h"
#include "Event.h"
#include "LayerStack.h"
//===============================================================================

namespace Aether
{
	class ImGuiLayer;
	class WindowResizeEvent;

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

		inline static Application& Get() { return *s_Instance; }
	private:

		bool OnWindowResize(WindowResizeEvent& e);

		// Layers all get owned by the layer stack, so we can create this as a raw pointer
		ImGuiLayer* m_ImGuiLayer = nullptr;

		// Main owner and container for application layers
		LayerStack m_LayerStack;

		// Quick and dirty Singleton-esque implementation
		static Application* s_Instance;
	};

	// External function to be defined in client applications (games)
	Application* CreateApplication();

};