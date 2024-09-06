//===============================================================================
// desc: Entry point for the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Renderer/IRenderer.h"
#ifdef USE_DX11
#include "Renderer/D3D/RendererDX11.h"
#endif
#ifdef USE_VULKAN
#include "Renderer/RendererVulkan.h" // To be implemented
#endif
//===============================================================================
#include <memory>
#include <iostream>
//===============================================================================
int main() {
    std::unique_ptr<IRenderer> renderer;

#ifdef USE_DX11
    renderer = std::make_unique<RendererDX11>();
#elif defined(USE_VULKAN)
    renderer = std::make_unique<RendererVulkan>();
#else
    std::cerr << "No rendering API defined. Please enable USE_DX11 or USE_VULKAN." << std::endl;
    return -1;
#endif

    if (!renderer->Initialize()) {
        std::cerr << "Renderer initialization failed." << std::endl;
        return -1;
    }

    // Main loop
    bool running = true;
    while (running) {
        // Handle window events here (e.g., using GLFW or another windowing library)
        // For simplicity, we'll exit immediately
        running = false;

        renderer->Render();
    }

    renderer->Terminate();
    return 0;
}
