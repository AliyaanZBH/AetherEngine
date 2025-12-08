//===============================================================================
// desc: The core rendering app that facilitates DLL exporting for use in the main AetherApp
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "CoreApp.h"
#include "IWindow.h"
#include "IRenderer.h"

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

AETHER_RESULT Aether::Application::Run()
{
    // A local instance that represents possible error codes.
    AETHER_RESULT ret;

    // Create unique, single instances of our key interfaces
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;

    // Set which type of window we are creating and pass in some data for it
    // Later in development, this will be read from a JSON config file so that the user can save and load settings, along with manually changing it from a GUI inside the application!
    Aether::IWindow::WinData wd =
    {
        .m_ClientWidth = 800,
        .m_ClientHeight = 600
        /*.m_Title = "AetherApp"*/      // Default title is Aether Engine
    };

#ifdef USE_GLFW
    window = std::make_unique<WindowGLFW>(wd);      // Calls initialise and catches errors inside with assert
#elif defined(USE_WIN32)
    renderer = std::make_unique<WinManWin32>();
#else
    #error No window API defined. Please enable USE_GLFW or USE_WIN32."
    ret = AETHER_FAIL;
#endif

    // Set the desired rendering API, based on the chosen macro.
#ifdef USE_DX11
    renderer = std::make_unique<RendererDX11>();
#elif defined(USE_DX12)
	renderer = std::make_unique<RendererDX12>();
#elif defined(USE_VULKAN)
    renderer = std::make_unique<RendererVulkan>();
#else
    #error No rendering API defined. Please enable USE_DX11 or USE_VULKAN.
    ret = AETHER_FAIL;
#endif

    // Init rendering API - catch errors out here with assert
    AETHER_ASSERT(renderer->Initialize(*window));

    // TEST: Try out events
    WindowResizeEvent e(1920u, 1080u);
    AETHER_TRACE(e);

    // The game loop!
    while (!window->WindowShouldClose()) {

        // Handle window events here (e.g., using GLFW or another windowing library)
        window->PollEvents();

        // Render our lovely frame!
        renderer->Render();
    }

    printf("\n\n\n");
    AETHER_CORE_INFO("Thanks for using Aether!\n");

    // Make sure we release our resources
    renderer->Terminate();
    window->Terminate();

    // Return the OK!
    return AETHER_OK;
}
