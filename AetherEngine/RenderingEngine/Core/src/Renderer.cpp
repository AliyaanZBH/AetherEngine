//===============================================================================
// desc: High-level API for applications using Aether to render games with ease
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Renderer.h"
#include "IRendererBackend.h"
#include "AetherUtils.h"

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
//===============================================================================

namespace Aether
{
    std::unique_ptr<IRendererBackend> Renderer::s_RendererBackend = nullptr;

	void Renderer::Initialise(IWindow& window)
	{
        AETHER_RESULT ar = AETHER_OK;

        // Set the desired rendering API, based on the chosen runtime enum. 
        std::string m_RendererString = "";
        switch (GraphicsContext::GetRenderAPI())
        {
        case eRenderAPI::kOpenGL:
        {
            s_RendererBackend = std::make_unique<RendererOpenGL>();
            m_RendererString = "OpenGL";
            break;
        }

        #ifdef USE_DX11
        case eRenderAPI::kDX11:
        {
            s_RendererBackend = std::make_unique<RendererDX11>();
            m_RendererString = "DirectX 11";
            break;
        }
        #endif

        #ifdef USE_DX12
        case eRenderAPI::kDX12:
        {
            s_RendererBackend = std::make_unique<RendererDX12>();
            m_RendererString = "DirectX 12";
            break;
        }
        #endif

        default:
        {
            ar = AETHER_FAIL;
            AETHER_ASSERT(ar, "No rendering API defined. Please enable one of the `USE_X` arguments to ensure that one is built and then select a valid desired rendering API.");
        }
        }

        AETHER_CORE_INFO("Using Renderer: {0}", m_RendererString);

        // Init rendering API - catch errors out here with assert
        AETHER_ASSERT(s_RendererBackend->Initialize(window));

	}
}