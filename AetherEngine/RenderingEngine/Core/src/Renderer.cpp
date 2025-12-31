#include "Renderer.h"
//===============================================================================
// desc: High-level API for applications using Aether to render games with ease
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Renderer.h"

#include "IRendererBackend.h"
#include "AetherUtils.h"
#include "Vertex.h"
#include "GraphicsContext.h"
#include "WindowContext.h"
#include "Window.h"

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
    std::vector<DrawCommand> Renderer::s_CommandQueue = {};
    Buffer* Renderer::s_QuadVertexBuffer = nullptr;
    Buffer* Renderer::s_QuadIndexBuffer = nullptr;
    VertexBufferView Renderer::s_QuadVBView = {};
    IndexBufferView Renderer::s_QuadIBView = {};

	void Renderer::Initialise()
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
        AETHER_ASSERT(s_RendererBackend->Initialize(Window::GetInterface()));

        // Create a high-level description of our render pipeline, and let the back-end take it away and build it.
        CreateBackendPipeline();

        // Create primitive geometry buffers that can be reused
        CreateQuadGeometry();

	}

    void Renderer::Terminate()
    {
        s_RendererBackend->Terminate();
    }

    void Renderer::BeginFrame()
    {
        s_RendererBackend->ClearFrame();
    }

    void Renderer::EndFrame()
    {
        // Dispatch all draw commands collected from the application this frame to the backend
        Dispatch();
        Flush();
        // Draw anything else we want! (Perhaps outdated at this point, but I like the idea of each renderer drendering something small and inconsequential like a small watermark as an easter egg)
        s_RendererBackend->Render();
        // Show completed frame
        s_RendererBackend->Present();
    }

    void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& colour)
    {
        // Register a draw command for quad geometry
        DrawCommand cmd;
        cmd.m_Type = eDrawCommandType::kQuad;
        cmd.m_VBV = &s_QuadVBView;
        cmd.m_IBV = &s_QuadIBView;
        cmd.m_Transform = transform;
        cmd.m_Colour = colour;
        s_CommandQueue.push_back(cmd);
    }

    void Renderer::Dispatch()
    {
        for (DrawCommand& cmd : s_CommandQueue)
        {
            s_RendererBackend->Submit(cmd);
            
            // Maybe save this for render passes like Opaque and Transparent?
            //switch (cmd.m_Type)
            //{
            //    case eDrawCommandType::kQuad:
            //    {
            //        s_RendererBackend->Submit(cmd);
            //    }
            //}
        }
    }

    void Renderer::Flush()
    {
        s_CommandQueue.clear();
    }

    void Renderer::InitImGui()
    {
        s_RendererBackend->InitImGui();
    }

    void Renderer::BeginImGuiRender()
    {
        s_RendererBackend->BeginImGuiRender();
    }

    void Renderer::EndImGuiRender()
    {
        s_RendererBackend->EndImGuiRender();
    }



    //
    // Private low-levelish implementations that are hidden from the application
    //


    void Renderer::CreateTriangleGeometry()
    {
    }

    void Renderer::CreateQuadGeometry()
    {

        // Create verts - position, colour
        // Clockwise verts! Clockwise winding order!
        Vertex quadVerts[] =
        {
            { {	-0.5f,		-0.5f,		0.5f,	1.f	}, {1.f, 0.f, 0.f, 1.f} },	// Bottom Left
            { {	-0.5f,		 0.5f,		0.5f,	1.f	}, {0.f, 1.f, 0.f, 1.f} },	// Top Left
            { {	 0.5f,		 0.5f,		0.5f,	1.f	}, {0.f, 0.f, 1.f, 1.f} },	// Top Right
            { {	 0.5f,		-0.5f,		0.5f,	1.f	}, {0.f, 1.f, 1.f, 1.f} }	// Bottom Right
        };

        BufferDesc quadVBDesc
        {
            .m_Data = quadVerts,
            .m_SizeInBytes = sizeof(quadVerts),
            .m_Type = eBufferType::kVertex,
            .m_CPUVisible = true
        };

        s_QuadVertexBuffer = s_RendererBackend->CreateBuffer(quadVBDesc);
        s_QuadVertexBuffer->Upload(quadVBDesc.m_Data, quadVBDesc.m_SizeInBytes);

        s_QuadVBView.m_Buffer = s_QuadVertexBuffer;
        s_QuadVBView.m_Stride = sizeof(Vertex);
        s_QuadVBView.m_Offset = 0;

        unsigned int quadIndices[6] = { 0, 1, 2, 2, 3, 0 };

        BufferDesc quadIBDesc
        {
            .m_Data = quadIndices,
            .m_SizeInBytes = sizeof(quadIndices),
            .m_Type = eBufferType::kIndex,
            .m_CPUVisible = true
        };

        s_QuadIndexBuffer = s_RendererBackend->CreateBuffer(quadIBDesc);
        s_QuadIndexBuffer->Upload(quadIBDesc.m_Data, quadIBDesc.m_SizeInBytes);
        s_QuadIBView.m_Buffer = s_QuadIndexBuffer;
        s_QuadIBView.m_Count = 6;
        s_QuadIBView.m_IndexSize = sizeof(unsigned int);
        s_QuadIBView.m_Offset = 0;

        // Finalise our upload to the renderer
        s_RendererBackend->FinalizeUploads();
    }

    void Renderer::CreateBackendPipeline()
    {
        // Define what layout we want our renderer to use and create pipelines for. Start with the vertex attributes

        VertexAttribute aPos
        {
            .m_Name = eShaderSemantic::kPosition,
            .m_Format = eVertexAttributeFormat::kFloat4,
            .m_Offset = 0   // Offset is optional and will be calculated by the layout constructor!
        };

        VertexAttribute aColour = { eShaderSemantic::kColour, eVertexAttributeFormat::kFloat4 };

        // Construct a layout with these attributes, offset and stride will be calculated internally
        VertexLayout layout({ aPos, aColour });

        // Grab shader library and register shaders or grab handle in the case that they've already been registered (not the case here, but could be when called later!)
        ShaderLibrary& shaders = ShaderLibrary::Get();
        ShaderDesc vsDesc
        {
            .m_Name = "VertexShader",       // No extensions, ideally we have identical shaders for both GLSL and HLSL. Let the renderer API figure out which one it needs to loads
            .m_ShaderStage = eShaderStage::kVertex
        };
        ShaderHandle vsHandle = shaders.Register("DefaultVertexShader", vsDesc);

        ShaderDesc psDesc
        {
            .m_Name = "PixelShader",
            .m_ShaderStage = eShaderStage::kPixel
        };
        ShaderHandle psHandle = shaders.Register("DefaultPixelShader", psDesc);

        PipelineDesc pipelineDesc =
        {
            .m_VertexShader = vsHandle,
            .m_PixelShader = psHandle,
            .m_Layout = layout
        };

        s_RendererBackend->CreatePipeline(pipelineDesc);
    }
}