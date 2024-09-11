//===============================================================================
// desc: The core rendering app that facilitates DLL exporting for use in the main AetherApp
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <memory>

#include "CoreApp.h"
#include "IWindow.h"
#include "IRenderer.h"

#ifdef USE_GLFW
#include "WinManGLFW.h"
#endif

#ifdef USE_DX11
#include "RendererDX11.h"
#endif

#ifdef USE_VULKAN
#include "Renderer/RendererVulkan.h" // To be implemented
#endif
//===============================================================================

AetherError Aether::Application::Run()
{
    // A local instance that represents possible error codes.
    AetherError ret;

    // Create unique, single instances of our key interfaces
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;

    // Set which type of window we are creating
    // Later in development, this will be read from a JSON config file so that the user can manually change it from a GUI inside the application!
#ifdef USE_GLFW
    window = std::make_unique<WinManGLFW>();
#elif defined(USE_WIN32)
    renderer = std::make_unique<WinManWin32>();
#else
    #error No window API defined. Please enable USE_GLFW or USE_WIN32."
    ret = 1;
#endif

    // Set the desired rendering API, based on the chosen macro.
#ifdef USE_DX11
    renderer = std::make_unique<RendererDX11>();
#elif defined(USE_VULKAN)
    renderer = std::make_unique<RendererVulkan>();
#else
    #error No rendering API defined. Please enable USE_DX11 or USE_VULKAN.
    ret = 1;
#endif

    // Set up winData struct. Again, this would be saved and loaded from a config file later in development
    int w = 800, h = 600;
    window->SetData(w, h, "Aether Engine");

    // Now try and initiate window
    if (!window->Initialize(window->GetData()))
        assert(false);
    // Init rendering API
    if (!renderer->Initialize(*window)) {
        throw "Renderer initialization failed.";
        ret = -1;
    }

    // The game loop!
    while (!window->WindowShouldClose()) {

        // Handle window events here (e.g., using GLFW or another windowing library)
        window->PollEvents();

        // Render our lovely frame!
        renderer->Render();
    }

    // Make sure we release our resources
    renderer->Terminate();

    // Return the OK!
    return AETHER_OK;
}
