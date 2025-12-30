//===============================================================================
// desc: The core engine app that facilitates general use for any application made with Aether!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "CoreApp.h"

#include "Log.h"
#include "Input.h"
#include "Renderer.h"
#include "Window.h"

#include "WindowContext.h"
#include "GraphicsContext.h"

#include "ImGuiLayer.h"
#include "AppEvent.h"
//===============================================================================
namespace Aether
{
#define BIND_APP_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        // A local instance that represents possible error codes.
        AETHER_RESULT ar = AETHER_OK;
        
        if (s_Instance != nullptr)
            AETHER_ASSERT(AETHER_FAIL, "An instance of the application is already running!");
        s_Instance = this;

        // Select rendering API
        GraphicsContext::SelectRenderAPI(eRenderAPI::kOpenGL);

        // Later in development, this will be read from a JSON config file so that the user can save and load settings, along with manually changing it from a GUI inside the application!
        WindowContext::WinData wd =
        {
            .m_ClientWidth = 800u,
            .m_ClientHeight = 600u
            /*.m_Title = "AetherApp"*/      // Default title is Aether Engine
        };

        // Initialise high-level window API, which sets up a low-level backend window platform. Inside this function you'll find the values for the default window size
        Window::Initialise(wd);
        
        // Initialise high-level rendering API, which in turn sets up the low-level backend with a default shader pipeline
        Renderer::Initialise();

		// Setup ImGui layer for the renderer too
		m_ImGuiLayer = new ImGuiLayer(GraphicsContext::GetRenderAPI());

		// Push the ImGui layer into the stack at the overlay point
		PushOverlay(m_ImGuiLayer);

        // Bind event callback for our window
        Window::SetEventCallback(BIND_APP_FN(OnEvent));
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
		// Let the renderer handle it's specific steps for resizing (recreating buffers, contexts, etc.)
        //m_Renderer->Resize(e.GetWidth(),e.GetHeight());
        return true;
    }

    void Application::OnEvent(Event& event)
    {
        // Just print the event for now
       // AETHER_CORE_TRACE("{0}", event);

        // Handle window resize in DirectX
        EventDispatcher dispatcher(event);

        // This magic function does a bit of type checking to ensure that only the correct event gets dispatched
        dispatcher.Dispatch<WindowResizeEvent>(BIND_APP_FN(OnWindowResize));

        // Pass event to layer stack to ensure event fires on correct layer
        m_LayerStack.HandleEvent(event);
    }


    AETHER_RESULT Application::Run()
    {
        // A local instance that represents possible error codes.
        AETHER_RESULT ar = AETHER_OK;

        // The game loop!
        while (!Window::ShouldClose())
        {
            // Start a new rendering frame!
            Renderer::BeginFrame();

            // Handle window events here (e.g., using GLFW or another backend window library)
            Window::Poll();

            // Refresh ImGui drawing context
            m_ImGuiLayer->Begin();

            // Update our layers!
            m_LayerStack.UpdateLayers();

            // Render our layers!
            m_LayerStack.RenderLayers();

            // Finalise ImGui drawing afterwards
            m_ImGuiLayer->End();

            // Finish rendering and present our lovely frame!
            Renderer::EndFrame();
        }

        printf("\n\n\n");
        AETHER_CORE_INFO("Thanks for using Aether!\n");

        // Make sure we release our resources manually if they aren't already tied in the destructor - everything in here will get deleted and have those called so no need to call things twice!
        Renderer::Terminate();

        // Return the OK!
        return AETHER_OK;
    }

};