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
	class ImGuiBaseLayer;

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
		inline IWindow& GetWindow() { return *m_Window; }
		inline IRenderer& GetRenderer() { return *m_Renderer; }

	private:
		std::unique_ptr<IWindow> m_Window;
		std::unique_ptr<IRenderer> m_Renderer;

		//std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		ImGuiBaseLayer* m_ImGuiLayer;

		LayerStack m_LayerStack;
		eRenderAPI m_CurrentRenderAPI = eRenderAPI::kOpenGL;

		// Quick and dirty Singleton-esque implementation
		static Application* s_Instance;
	};

	// External function to be defined in client applications (games)
	Application* CreateApplication();

};