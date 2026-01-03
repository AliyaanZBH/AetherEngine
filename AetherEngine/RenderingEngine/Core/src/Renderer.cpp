//===============================================================================
// desc: High-level API for applications using Aether to render games with ease
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Renderer.h"

#include "IRendererBackend.h"
#include "AetherUtils.h"
#include "Vertex.h"
#include "GraphicsContext.h"
#include "GraphicsCommon.h"
#include "WindowContext.h"
#include "Window.h"
#include "Camera.h"

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

    // Constant buffer instances
    Buffer* Renderer::s_PerFrameBuffer = nullptr;
    ConstantBufferView Renderer::s_PerFrameCBView = {};
    Buffer* Renderer::s_PerDrawBuffer = nullptr;
    ConstantBufferView Renderer::s_PerDrawCBView = {};

    // Geometry buffer instances
    Buffer* Renderer::s_TriVertexBuffer = nullptr;
    Buffer* Renderer::s_TriIndexBuffer = nullptr;
    VertexBufferView* Renderer::s_TriVBView = nullptr;
    IndexBufferView* Renderer::s_TriIBView = nullptr;

    Buffer* Renderer::s_QuadVertexBuffer = nullptr;
    Buffer* Renderer::s_QuadIndexBuffer = nullptr;
    VertexBufferView* Renderer::s_QuadVBView = nullptr;
    IndexBufferView* Renderer::s_QuadIBView = nullptr;

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
        CreateTriangleGeometry();
        CreateQuadGeometry();

        // Finalise our uploads to the renderer
        s_RendererBackend->FinalizeUploads();

        // Reserve some space for our command queue up-front, to avoid re-allocations
        s_CommandQueue.reserve(128);

        // Create constant buffers for per-frame and per-draw data
        s_PerFrameBuffer = CreateConstantBuffer<PerFrameData>(eBufferType::kConstantPerFrame, &s_PerFrameCBView, 0);
        s_PerDrawBuffer = CreateConstantBuffer<PerDrawData>(eBufferType::kConstantPerDraw, &s_PerDrawCBView, 1);

	}

    void Renderer::Terminate()
    {
        // Delete all buffers we allocated
        delete s_TriVertexBuffer;
        delete s_TriIndexBuffer;
        delete s_QuadVertexBuffer;
        delete s_QuadIndexBuffer;

        // Tear down renderer backend too
        s_RendererBackend->Terminate();
    }

    void Renderer::Resize(const uint16_t width, const uint16_t height)
    {
        s_RendererBackend->Resize(width, height);
    }

    void Renderer::BeginFrame(const Camera& camera)
    {
        // Clear frame and ensure we are safe to continue rendering and submitting data to the GPU
        s_RendererBackend->ClearFrame();

        // Now upload per-frame data to the GPU
        PerFrameData data;
        data.m_ViewProj = camera.GetViewProj();

        s_PerFrameBuffer->Upload(&data, sizeof(PerFrameData));
        s_RendererBackend->BindFrameConstants(&s_PerFrameCBView);
    }

    void Renderer::Render()
    {
        // Dispatch all draw commands collected from the application this frame to the backend
        Dispatch();
        Flush();
        // Draw anything else we want! (Perhaps outdated at this point, but I like the idea of each renderer drendering something small and inconsequential like a small watermark as an easter egg)
        s_RendererBackend->Render();
    }

    void Renderer::EndFrame()
    {
        // Show completed frame - ImGui will have finished by this point.
        s_RendererBackend->Present();
    }

    //
    // Drawing Functions!
    //

    void Renderer::DrawLine()
    {
    }

    void Renderer::DrawTriangle(const Transform& transform, const glm::vec4& colour)
    {
        DrawCommand cmd;
        cmd.m_Type = eDrawCommandType::kTri;
        cmd.m_VBV = s_TriVBView;
        cmd.m_IBV = s_TriIBView;
        cmd.m_ModelMatrix = transform.CreateModelMatrix();
        cmd.m_SolidColour = colour;
        s_CommandQueue.push_back(cmd);
    }

    void Renderer::DrawQuad(const Transform& transform, const glm::vec4& colour)
    {
        // Register a draw command for geometry
        DrawCommand cmd;
        cmd.m_Type = eDrawCommandType::kQuad;
        cmd.m_VBV = s_QuadVBView;
        cmd.m_IBV = s_QuadIBView;
        cmd.m_ModelMatrix = transform.CreateModelMatrix();
        cmd.m_SolidColour = colour;
        s_CommandQueue.push_back(cmd);
    }

    void Renderer::DrawCircle()
    {
    }

    void Renderer::DrawMesh()
    {
    }

    //
    // Dispatch rendering
    //

    void Renderer::Dispatch()
    {

        for (DrawCommand& cmd : s_CommandQueue)
        {
            // Update constant buffer with data for this draww
            PerDrawData data;
            data.m_ModelMatrix = cmd.m_ModelMatrix;
            data.m_Colour = cmd.m_SolidColour;
            s_PerDrawBuffer->Upload(&data, sizeof(PerDrawData));

            // Submit the view on this buffer together with the command;
            s_RendererBackend->Submit(cmd, &s_PerDrawCBView);

            // Maybe save this for render passes like Opaque and Transparent?
            //switch (cmd.m_Type)
            //{
            //    case eDrawCommandType::kQuad:
            //    {
            //        s_RendererBackend->Submit(cmd);
            //    }
            //}
        };


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
        // Create verts - position, colour
        // Clockwise verts! Clockwise winding order!
        Vertex triVerts[] =
        {
            { {	-0.5f,		-0.5f,		0.5f,	1.f	}, {1.f, 0.f, 0.f, 1.f} },	// Bottom Left
            { {	 0.0f,		 0.5f,		0.5f,	1.f	}, {0.f, 1.f, 0.f, 1.f} },	// Top
            { {	 0.5f,		-0.5f,		0.5f,	1.f	}, {0.f, 0.f, 1.f, 1.f} },	// Bottom Right
        };

        BufferDesc triVBDesc
        {
            .m_Data = triVerts,
            .m_SizeInBytes = sizeof(triVerts),
            .m_Type = eBufferType::kVertex,
            .m_CPUVisible = false
        };

        s_TriVertexBuffer = s_RendererBackend->CreateBuffer(triVBDesc);
        s_TriVertexBuffer->Upload(triVBDesc.m_Data, triVBDesc.m_SizeInBytes);

        s_TriVBView = new VertexBufferView();
        s_TriVBView->m_Buffer = s_TriVertexBuffer;
        s_TriVBView->m_Stride = sizeof(Vertex);
        s_TriVBView->m_Offset = 0;

        unsigned int triIndices[3] = { 0, 1, 2 };

        BufferDesc triIBDesc
        {
            .m_Data = triIndices,
            .m_SizeInBytes = sizeof(triIndices),
            .m_Type = eBufferType::kIndex,
            .m_CPUVisible = false
        };

        s_TriIndexBuffer = s_RendererBackend->CreateBuffer(triIBDesc);
        s_TriIndexBuffer->Upload(triIBDesc.m_Data, triIBDesc.m_SizeInBytes);
        s_TriIBView = new IndexBufferView();
        s_TriIBView->m_Buffer = s_TriIndexBuffer;
        s_TriIBView->m_Count = 3;
        s_TriIBView->m_IndexSize = sizeof(unsigned int);
        s_TriIBView->m_Offset = 0;
    }

    void Renderer::CreateQuadGeometry()
    {
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
            .m_CPUVisible = false
        };

        s_QuadVertexBuffer = s_RendererBackend->CreateBuffer(quadVBDesc);
        s_QuadVertexBuffer->Upload(quadVBDesc.m_Data, quadVBDesc.m_SizeInBytes);

        s_QuadVBView = new VertexBufferView();
        s_QuadVBView->m_Buffer = s_QuadVertexBuffer;
        s_QuadVBView->m_Stride = sizeof(Vertex);
        s_QuadVBView->m_Offset = 0;

        unsigned int quadIndices[6] = { 0, 1, 2, 2, 3, 0 };

        BufferDesc quadIBDesc
        {
            .m_Data = quadIndices,
            .m_SizeInBytes = sizeof(quadIndices),
            .m_Type = eBufferType::kIndex,
            .m_CPUVisible = false
        };

        s_QuadIndexBuffer = s_RendererBackend->CreateBuffer(quadIBDesc);
        s_QuadIndexBuffer->Upload(quadIBDesc.m_Data, quadIBDesc.m_SizeInBytes);
        s_QuadIBView = new IndexBufferView();
        s_QuadIBView->m_Buffer = s_QuadIndexBuffer;
        s_QuadIBView->m_Count = 6;
        s_QuadIBView->m_IndexSize = sizeof(unsigned int);
        s_QuadIBView->m_Offset = 0;
    }

    template<typename T>
    Buffer* Renderer::CreateConstantBuffer( const eBufferType type, ConstantBufferView* cbv, uint8_t slot)
    {
        // Create a re-usable and generic constant buffer
        BufferDesc conBufDesc;
        conBufDesc.m_Type = type;
        conBufDesc.m_SizeInBytes = sizeof(T);
        conBufDesc.m_CPUVisible = true;

        // We'll crash in DX if we try to create a constant buffer with empty data, so supply basic identity data for now
        T tmpData = {};
        conBufDesc.m_Data = &tmpData;

        Buffer* buf = s_RendererBackend->CreateBuffer(conBufDesc);
        // Fill in the view too
        cbv->m_Buffer = buf;
        cbv->m_Size = buf->GetSize();
        cbv->m_Slot = slot;
        
        return buf;
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