#pragma once

//===============================================================================
// desc: Interface for an abstract renderer, usable by DX11,Vulkan, etc.
// auth: Aliyaan Zulfiqar
//===============================================================================

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual bool Initialize() = 0;
    virtual void Render() = 0;
    virtual void Terminate() = 0;
};