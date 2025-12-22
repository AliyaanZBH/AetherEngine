#include "CoreApp.h"
//===============================================================================
// desc: The core engine app that facilitates general use for any application made with Aether!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "CoreApp.h"
#include "Log.h"

#ifdef USE_GLFW
#include "WindowGLFW.h"
#endif

#ifdef USE_OPENGL
#include "RendererOpenGL.h"
#endif

#ifdef USE_DX11
#include "RendererDX11.h"
#endif

#ifdef USE_DX12
#include "RendererDX12.h"
#endif

#ifdef USE_VULKAN
#include "Renderer/RendererVulkan.h" // To be implemented
#endif

#include "AppEvent.h"
#include "ImGuiLayer.h"
#include "Input.h"
#include "GraphicsContext.h"
#include "Pipeline.h"
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

        // Set which type of window we are creating and pass in some data for it
        // Later in development, this will be read from a JSON config file so that the user can save and load settings, along with manually changing it from a GUI inside the application!
        IWindow::WinData wd =
        {
            .m_ClientWidth = 800,
            .m_ClientHeight = 600
            /*.m_Title = "AetherApp"*/      // Default title is Aether Engine
        };

    #ifdef USE_GLFW
        m_Window = std::make_unique<WindowGLFW>(wd);      // Calls initialise and catches errors inside with assert
    #elif defined(USE_WIN32)
        m_Window = std::make_unique<WinManWin32>();
    #else
    #error No window API defined. Please enable USE_GLFW or USE_WIN32."
        ar = AETHER_FAIL;
    #endif

        // Set the desired rendering API, based on the chosen runtime enum. 
        switch (m_CurrentRenderAPI)
        {
            case eRenderAPI::kOpenGL:
            {
                m_Renderer = std::make_unique<RendererOpenGL>();
                break;
            }

            #ifdef USE_DX11
            case eRenderAPI::kDX11:
            {
                m_Renderer = std::make_unique<RendererDX11>();
                break;
            }
            #endif

            #ifdef USE_DX12
            case eRenderAPI::kDX12:
            {
                m_Renderer = std::make_unique<RendererDX12>();
                break;
            }
            #endif

            default:
            {
                ar = AETHER_FAIL;
                AETHER_ASSERT(ar, "No rendering API defined. Please enable one of the `USE_X` arguments to ensure that one is built and then select a valid desired rendering API.");
            }
        }

		// Init rendering API - catch errors out here with assert
		AETHER_ASSERT(m_Renderer->Initialize(*m_Window));

        // Create default pipeline for the renderer
        CreatePipeline();

		// Setup ImGui layer for the renderer too
		m_ImGuiLayer = new ImGuiLayer(m_CurrentRenderAPI);

		// Push the ImGui layer into the stack at the overlay point
		PushOverlay(m_ImGuiLayer);

        // Bind event callback for our window
        m_Window->SetEventCallback(BIND_APP_FN(OnEvent));
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

    void Application::CreatePipeline()
    {
        // Define what layout we want our renderer to use and create pipelines for
        VertexLayout layout;
        layout.m_Stride = sizeof(Vertex);

        VertexAttribute aPos
        {
            .m_Name = eShaderSemantic::kPosition,
            .m_Format = eVertexAttributeFormat::kFloat4,
            .m_Offset = 0
        };

        layout.m_Attributes =
        {
            aPos,
            { eShaderSemantic::kColour, eVertexAttributeFormat::kFloat4,offsetof(Vertex, m_Colour) }
        };

        // Grab shader library and register shaders or grab handle in the case that they've already been registered (not the case here, but could be when called later!)
        ShaderLibrary& shaders = ShaderLibrary::Get();
        ShaderDesc vsDesc
        {
            .m_Name = "VertexShader",
            .m_ShaderStage = eShaderStage::kVertex
        };
        ShaderHandle vsHandle = shaders.Register("DefaultVertexShader", vsDesc);

        ShaderDesc psDesc
        {
            .m_Name = "PixelShader",
            .m_ShaderStage = eShaderStage::kPixel
        };
        ShaderHandle psHandle = shaders.Register("DefaultIndexShader", psDesc);

        PipelineDesc pipelineDesc =
        {
            .m_VertexShader = vsHandle,
            .m_PixelShader = psHandle,
            .m_Layout = layout
        };
        m_Renderer->CreatePipeline(pipelineDesc);
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
		// Let the renderer handle it's specific steps for resizing (recreating buffers, contexts, etc.)
        m_Renderer->Resize(e.GetWidth(),e.GetHeight());
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
        while (!m_Window->WindowShouldClose())
        {
            // Clear frame!
            m_Renderer->ClearFrame();

            // Handle window events here (e.g., using GLFW or another windowing library)
            m_Window->PollEvents();

            // Refresh ImGui drawing context
            m_ImGuiLayer->Begin();

            // Update our layers! Eventually, the renderer will tie in to this aswell as it will render each layer. ImGui renders here too, which is why we clear frame and begin earlier.
            m_LayerStack.UpdateLayers();

            m_LayerStack.RenderLayers();

            // Draw anything else we want!
            m_Renderer->Render();

            // Finalise ImGui drawing afterwards
            m_ImGuiLayer->End();

            // Present our finished lovely frame!
            m_Renderer->Present();
        }

        printf("\n\n\n");
        AETHER_CORE_INFO("Thanks for using Aether!\n");

        // Make sure we release our resources manually if they aren't already tied in the destructor - everything in here will get deleted and have those called so no need to call things twice!
        m_Renderer->Terminate();

        // Return the OK!
        return AETHER_OK;
    }

};