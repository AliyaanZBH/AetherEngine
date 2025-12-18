#pragma once
//===============================================================================
// desc: Interface for an abstract renderer, usable by DX11,Vulkan, etc.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IWindow.h"
#include "AetherUtils.h"
//===============================================================================

namespace Aether
{
    class AETHER_API IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual AETHER_RESULT Initialize(IWindow& window) = 0;
        virtual void ClearFrame() = 0;
        virtual void Render() = 0;
        virtual void Present() = 0;
        virtual void Resize(int newWidth, int newHeight) = 0;
        virtual void Terminate() = 0;
        virtual void* GetNativeDevice() = 0;
        virtual void* GetNativeContext() = 0;

        // To be called by ImGui layer
        virtual void InitImGui() = 0;
        virtual void BeginImGuiRender() = 0;
        virtual void EndImGuiRender() = 0;
    };

    enum class eRenderAPI
    {
        kOpenGL,
        kDX11,
        kDX12
    };
}