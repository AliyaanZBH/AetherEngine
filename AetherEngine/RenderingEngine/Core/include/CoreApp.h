#pragma once
//===============================================================================
// desc: The core engine app that facilitates general use for any application made with Aether!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "AetherUtils.h"
#include "IWindow.h"
#include "IRenderer.h"
#include "Event.h"
#include "LayerStack.h"
#include "GraphicsContext.h"
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
		inline IWindow& GetWindow() { return *m_Window; }
		inline IRenderer& GetRenderer() { return *m_Renderer; }
		inline const eRenderAPI GetRenderAPI() { return m_CurrentRenderAPI; }

	private:

		void CreatePipeline();
		bool OnWindowResize(WindowResizeEvent& e);

		std::unique_ptr<IWindow> m_Window;
		std::unique_ptr<IRenderer> m_Renderer;

		// Layers all get owned by the layer stack, so we can create this as a raw pointer
		ImGuiLayer* m_ImGuiLayer = nullptr;

		LayerStack m_LayerStack;
		eRenderAPI m_CurrentRenderAPI = eRenderAPI::kDX11;

		// Quick and dirty Singleton-esque implementation
		static Application* s_Instance;
	};

	// External function to be defined in client applications (games)
	Application* CreateApplication();

};