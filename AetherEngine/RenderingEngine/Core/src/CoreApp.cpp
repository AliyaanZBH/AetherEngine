//===============================================================================
// desc: The core rendering app that facilitates DLL exporting for use in the main AetherApp
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "CoreApp.h"

#ifdef USE_GLFW
#include "WindowGLFW.h"
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
#include "Log.h"
//===============================================================================
namespace Aether
{
#define BIND_APP_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application::Application()
    {
        // A local instance that represents possible error codes.
        AETHER_RESULT ar = AETHER_OK;

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

        // Set the desired rendering API, based on the chosen macro.
    #ifdef USE_DX11
        m_Renderer = std::make_unique<RendererDX11>();
    #elif defined(USE_DX12)
        m_Renderer = std::make_unique<RendererDX12>();
    #elif defined(USE_VULKAN)
        m_Renderer = std::make_unique<RendererVulkan>();
    #else
    #error No rendering API defined. Please enable USE_DX11 or USE_VULKAN.
        ar = AETHER_FAIL;
    #endif

        // Bind event callback for our window
        m_Window->SetEventCallback(BIND_APP_FN(OnEvent));
    }


    void Application::OnEvent(Event& event)
    {
        // Just print the event for now
        AETHER_CORE_INFO("{0}", event);
    }

    AETHER_RESULT Application::Run()
    {
        // A local instance that represents possible error codes.
        AETHER_RESULT ar = AETHER_OK;

        // Init rendering API - catch errors out here with assert
        AETHER_ASSERT(m_Renderer->Initialize(*m_Window));

        // TEST: Try out events
        WindowResizeEvent e(1920u, 1080u);
        AETHER_TRACE(e);

        // The game loop!
        while (!m_Window->WindowShouldClose()) {

            // Handle window events here (e.g., using GLFW or another windowing library)
            m_Window->PollEvents();

            // Render our lovely frame!
            m_Renderer->Render();
        }

        printf("\n\n\n");
        AETHER_CORE_INFO("Thanks for using Aether!\n");

        // Make sure we release our resources
        m_Renderer->Terminate();
        m_Window->Terminate();

        // Return the OK!
        return AETHER_OK;
    }

};