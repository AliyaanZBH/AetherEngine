//===============================================================================
// desc: Entry point for the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <memory>

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

