//===============================================================================
// desc: The core engine app that facilitates general use for any application made with Aether!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "CoreApp.h"

#include "AetherTime.h"
#include "Log.h"
#include "Input.h"
#include "Renderer.h"
#include "Window.h"

#include "WindowContext.h"
#include "GraphicsContext.h"

#include "Camera.h"

#include "ImGuiLayer.h"
#include "AppEvent.h"
#include "MouseEvent.h"
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
        GraphicsContext::SelectRenderAPI(eRenderAPI::kDX11);

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

        // Create a camera too
        m_Camera = new Camera(eProjectionType::kPerspective);

        // Set aspect ratio
        const float aspect = (float)wd.m_ClientWidth / (float)wd.m_ClientHeight;
        m_Camera->SetAspectRatio(aspect);

        // Move it back a tad
        m_Camera->SetPosition({ 0.f, 0.f, -1.f });
       
		// Setup ImGui layer
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
        uint16_t width = e.GetWidth();
        uint16_t height = e.GetHeight();

		// Let the renderer handle specific steps for resizing (recreating buffers, contexts, etc.)
        Renderer::Resize(width, height);

        // Also update camera aspect ratio
        const float aspect = (float)width / height;
        m_Camera->SetAspectRatio(aspect);

        return true;
    }

    void Application::OnEvent(Event& event)
    {
        // Just print the event for now
        // AETHER_CORE_TRACE("{0}", event);

        // Handle window resize in DirectX
        EventDispatcher dispatcher(event);

        // This magic function does a bit of static type checking to ensure that only the correct event gets fired by the correct layer
        dispatcher.Dispatch<WindowResizeEvent>(BIND_APP_FN(OnWindowResize));

        // Handle mouse locking
        dispatcher.Dispatch<MouseLockEvent>([this] (MouseLockEvent& e)
        {
            Window::SetCursorLocked(e.ShouldLock());
            return true;
        });

        // Also ensure that when focus is lost, the mouse is always unlocked so it never gets stuck off screen
        dispatcher.Dispatch<WindowFocusEvent>([this] (WindowFocusEvent& e)
        {
            if (!e.IsFocused())
                Window::SetCursorLocked(false);
            return false;   // Return false as we might have other things that want to handle this event too
        });

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
            Time::Update();

            // Start a new rendering frame!
            Renderer::BeginFrame(*m_Camera);

            // Handle window events here (e.g., using GLFW or another backend window library)
            Window::Poll();

            // Refresh ImGui drawing context
            m_ImGuiLayer->Begin();

            // Update our layers!
            m_LayerStack.UpdateLayers();

            // Render our layers!
            m_LayerStack.RenderLayers();

            // Finalize rendering by submitting all registered draw commands to the backend
            Renderer::Render();

            // Finalise ImGui drawing last, as it will invalidate renderer state.
            m_ImGuiLayer->End();

            // Now present our lovely frame!
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
