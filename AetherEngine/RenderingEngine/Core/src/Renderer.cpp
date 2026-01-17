#include "Renderer.h"
#include "Renderer.h"
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
#include "GraphicsCommon.h"
#include "WindowContext.h"
#include "Window.h"
#include "Camera.h"
#include "Material.h"

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
    std::vector<DrawCommand> Renderer::s_FlatColourCommandQueue = {};
    std::vector<DrawCommand> Renderer::s_LitColourCommandQueue = {};
    std::vector<DrawCommand> Renderer::s_TexturedCommandQueue = {};

    const std::string Renderer::s_FlatColourPipeName = "SolidColour";
    const std::string Renderer::s_LitColourPipeName = "BlinnPhong";

    // Materials
    Material* Renderer::s_FlatColourMat;
    Material* Renderer::s_LitColourMat;

    std::vector<uint8_t> Renderer::s_FlatColourMaterialDataCPU;
    Buffer* Renderer::s_FlatColourMaterialDataGPU = nullptr;

    std::vector<uint8_t> Renderer::s_LitColourMaterialDataCPU;
    Buffer* Renderer::s_LitColourMaterialDataGPU = nullptr;

    uint32_t Renderer::s_NextMatIdx = 0u;

    // Constant buffer instances
    Buffer* Renderer::s_PerFrameBuffer = nullptr;
    ConstantBufferView Renderer::s_PerFrameCBView = {};
    Buffer* Renderer::s_PerDrawBuffer = nullptr;
    ConstantBufferView Renderer::s_PerDrawCBView = {};

    // Geometry buffer instances
    GeometryBuffer* Renderer::s_TriGeoBuffer = nullptr;
    GeometryBuffer* Renderer::s_QuadGeoBuffer = nullptr;
    GeometryBuffer* Renderer::s_BoxGeoBuffer = nullptr;


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

        // Create a high-level description of our render pipeline, and let the back-end take it away and build it. Also builds materials here.
        CreateBackendPipeline();

        // Create primitive geometry buffers that can be reused
        CreateTriangleGeometry();
        CreateQuadGeometry();
        CreateBoxGeometry();

        // Finalise our uploads to the renderer - this sets up any final state each backend needs (cmd list exec in DX12, VAO creation for geometry in OGL)
        s_RendererBackend->FinalizeUploads();

        // Reserve some space for our command queue up-front, to avoid re-allocations
        s_FlatColourCommandQueue.reserve(128);
        s_LitColourCommandQueue.reserve(128);
        s_TexturedCommandQueue.reserve(128);

        // Create constant buffers for per-frame and per-draw data
        s_PerFrameBuffer = CreateConstantBuffer<PerFrameData>(eBufferType::kConstantPerFrame, &s_PerFrameCBView, 0);
        s_PerDrawBuffer = CreateConstantBuffer<PerDrawData>(eBufferType::kConstantPerDraw, &s_PerDrawCBView, 1);

	}

    void Renderer::Terminate()
    {
        // Delete all buffers we allocated
        delete s_TriGeoBuffer;
        delete s_QuadGeoBuffer;

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
        data.m_CameraPos = glm::vec4(camera.GetPosition(), 1.0f);

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

    void Renderer::Draw(const eDrawGeoType drawType, const Transform& transform, MaterialInstance* mat)
    {
        DrawCommand cmd;
        cmd.m_ModelMatrix = transform.CreateModelMatrix();
        cmd.m_MaterialInstance = mat;

        // Switch on draw type and bind correct geo buffers
        switch (drawType)
        {
            case eDrawGeoType::kTri:
            {
                cmd.m_Type = eDrawGeoType::kTri;
                cmd.m_VBV = &s_TriGeoBuffer->vbView;
                cmd.m_IBV = &s_TriGeoBuffer->ibView;
                break;
            }

            case eDrawGeoType::kQuad:
            {
                cmd.m_Type = eDrawGeoType::kQuad;
                cmd.m_VBV = &s_QuadGeoBuffer->vbView;
                cmd.m_IBV = &s_QuadGeoBuffer->ibView;
                break;
            }

            case eDrawGeoType::kCube:
            {
                cmd.m_Type = eDrawGeoType::kCube;
                cmd.m_VBV = &s_BoxGeoBuffer->vbView;
                cmd.m_IBV = &s_BoxGeoBuffer->ibView;
                break;
            }

            default:
            {
                break;
            }
        }

        // Finally, figure out which render pass / pipeline we want to push this into based on material
        switch (mat->GetMaterialRef().GetType())
        {
            case eMaterialType::kSolidColour:
            {
                s_FlatColourCommandQueue.push_back(cmd);
                break;
            }

            case eMaterialType::kBlinnPhong:
            {
                s_LitColourCommandQueue.push_back(cmd);
                break;
            }

            case eMaterialType::kTextured:
            {
                s_TexturedCommandQueue.push_back(cmd);
                break;
            }
        }
    }

    void Renderer::UploadMaterialInstance(MaterialInstance& instance)
    {
        const Material& material = instance.GetMaterialRef();
        uint32_t stride = material.GetDataStride();

        uint32_t index = s_NextMatIdx++;
        instance.SetMaterialIndex(index);

        size_t offset = index * stride;

        // Grab correct material buffer
        std::vector<uint8_t>* cpuBuffer;
        Buffer* gpuBuffer = nullptr;
        switch (instance.GetType())
        {
            case eMaterialType::kSolidColour:
            {
                cpuBuffer = &s_FlatColourMaterialDataCPU;
                gpuBuffer = s_FlatColourMaterialDataGPU;
                break;
            }
            case eMaterialType::kBlinnPhong:
            {
                cpuBuffer = &s_LitColourMaterialDataCPU;
                gpuBuffer = s_LitColourMaterialDataGPU;
                break;
            }
        }

        // Update CPU side data
        if (cpuBuffer->size() < offset + stride)
            cpuBuffer->resize(offset + stride);

        memcpy(cpuBuffer->data() + offset, instance.GetRawData(), stride);

        // Upload to GPU structured buffer
        gpuBuffer->Upload(cpuBuffer->data(), cpuBuffer->size());
    }

    void Renderer::UpdateMaterialInstance(MaterialInstance& inst)
    {
        if (!inst.IsDirty())
            return;

        const uint32_t index = inst.GetMaterialIndex();
        const uint32_t size = inst.GetMaterialRef().GetDataStride();

        // Grab correct material buffer
        Buffer* gpuBuffer;
        switch (inst.GetType())
        {
            case eMaterialType::kSolidColour:
            {
                gpuBuffer = s_FlatColourMaterialDataGPU;
                break;
            }
            case eMaterialType::kBlinnPhong:
            {
                gpuBuffer = s_LitColourMaterialDataGPU;
                break;
            }
        }

        gpuBuffer->Upload(
            inst.GetRawData(),          // Pointer to ONE material struct
            size,
            index * size,               // Byte offset to the correct element
            false                       // No discarding of the buffer! Just cleanly overwrite the region belonging to this material
        );
        inst.ClearDirty();
    }

    //
    // Dispatch rendering
    //

    void Renderer::Dispatch()
    {

        // Iterate through our command queues and fire off draw commands. Set pipeline / render pass state once at the start
        s_RendererBackend->BindPipeline(PipelineLibrary::Get().GetHandle(s_FlatColourPipeName));

        // Pass up our material buffer as a global resource for this pass
        s_RendererBackend->BindGlobalResources(s_FlatColourMaterialDataGPU);

        // TODO: Maybe further improve this by seperating render passes by Opaque and Transparent?

        for (DrawCommand& cmd : s_FlatColourCommandQueue)
        {
            // Update constant buffer with data for this objects material
            PerDrawData data;
            data.m_ModelMatrix = cmd.m_ModelMatrix;
            data.m_MaterialIndex = cmd.m_MaterialInstance->GetMaterialIndex();
            s_PerDrawBuffer->Upload(&data, sizeof(data));  
            
            // Submit the draw command for rendering, along with this draw calls CBV
            s_RendererBackend->Submit(cmd, &s_PerDrawCBView);
        };

        // Repeat for all pipelines
        s_RendererBackend->BindPipeline(PipelineLibrary::Get().GetHandle(s_LitColourPipeName));
        s_RendererBackend->BindGlobalResources(s_LitColourMaterialDataGPU);

        for (DrawCommand& cmd : s_LitColourCommandQueue)
        {
            // Update constant buffer with data for this objects material
            PerDrawData data;
            data.m_ModelMatrix = cmd.m_ModelMatrix;
            data.m_MaterialIndex = cmd.m_MaterialInstance->GetMaterialIndex();
            s_PerDrawBuffer->Upload(&data, sizeof(data));

            // Submit the draw command for rendering, along with this draw calls CBV
            s_RendererBackend->Submit(cmd, &s_PerDrawCBView);
        };
    }

    void Renderer::Flush()
    {
        s_FlatColourCommandQueue.clear();
        s_LitColourCommandQueue.clear();
        s_TexturedCommandQueue.clear();
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
            { {	-0.5f,		-0.5f,		0.5f,	1.f	}, { 0.f, 0.f, 1.f } },	// Bottom Left
            { {	 0.0f,		 0.5f,		0.5f,	1.f	}, { 0.f, 0.f, 1.f } },	// Top
            { {	 0.5f,		-0.5f,		0.5f,	1.f	}, { 0.f, 0.f, 1.f } }	// Bottom Right
        };

        BufferDesc triVBDesc
        {
            .m_Data = triVerts,
            .m_SizeInBytes = sizeof(triVerts),
            .m_Type = eBufferType::kVertex,
            .m_CPUVisible = false
        };

        s_TriGeoBuffer = new GeometryBuffer();
        s_TriGeoBuffer->vbView.m_Buffer = s_RendererBackend->CreateBuffer(triVBDesc);
        s_TriGeoBuffer->vbView.m_Buffer->Upload(triVBDesc.m_Data, triVBDesc.m_SizeInBytes);
        s_TriGeoBuffer->vbView.m_Stride = sizeof(Vertex);
        s_TriGeoBuffer->vbView.m_Offset = 0;

        unsigned int triIndices[3] = { 0, 1, 2 };

        BufferDesc triIBDesc
        {
            .m_Data = triIndices,
            .m_SizeInBytes = sizeof(triIndices),
            .m_Type = eBufferType::kIndex,
            .m_CPUVisible = false
        };

        s_TriGeoBuffer->ibView.m_Buffer = s_RendererBackend->CreateBuffer(triIBDesc);
        s_TriGeoBuffer->ibView.m_Buffer->Upload(triIBDesc.m_Data, triIBDesc.m_SizeInBytes);
        s_TriGeoBuffer->ibView.m_Count = 3;
        s_TriGeoBuffer->ibView.m_IndexSize = sizeof(unsigned int);
        s_TriGeoBuffer->ibView.m_Offset = 0;
    }

    void Renderer::CreateQuadGeometry()
    {
        Vertex quadVerts[] =
        {
            { {	-0.5f,		-0.5f,		0.5f,	1.f	}, { 0.f, 0.f, 1.f } },	// Bottom Left
            { {	-0.5f,		 0.5f,		0.5f,	1.f	}, { 0.f, 0.f, 1.f } },	// Top Left
            { {	 0.5f,		 0.5f,		0.5f,	1.f	}, { 0.f, 0.f, 1.f } },	// Top Right
            { {	 0.5f,		-0.5f,		0.5f,	1.f	}, { 0.f, 0.f, 1.f } }	// Bottom Right
        };

        BufferDesc quadVBDesc
        {
            .m_Data = quadVerts,
            .m_SizeInBytes = sizeof(quadVerts),
            .m_Type = eBufferType::kVertex,
            .m_CPUVisible = false
        };

        s_QuadGeoBuffer = new GeometryBuffer();
        s_QuadGeoBuffer->vbView.m_Buffer = s_RendererBackend->CreateBuffer(quadVBDesc);
        s_QuadGeoBuffer->vbView.m_Buffer->Upload(quadVBDesc.m_Data, quadVBDesc.m_SizeInBytes);
        s_QuadGeoBuffer->vbView.m_Stride = sizeof(Vertex);
        s_QuadGeoBuffer->vbView.m_Offset = 0;

        unsigned int quadIndices[6] = { 0, 1, 2, 2, 3, 0 };

        BufferDesc quadIBDesc
        {
            .m_Data = quadIndices,
            .m_SizeInBytes = sizeof(quadIndices),
            .m_Type = eBufferType::kIndex,
            .m_CPUVisible = false
        };

        s_QuadGeoBuffer->ibView.m_Buffer = s_RendererBackend->CreateBuffer(quadIBDesc);
        s_QuadGeoBuffer->ibView.m_Buffer->Upload(quadIBDesc.m_Data, quadIBDesc.m_SizeInBytes);
        s_QuadGeoBuffer->ibView.m_Count = 6;
        s_QuadGeoBuffer->ibView.m_IndexSize = sizeof(unsigned int);
        s_QuadGeoBuffer->ibView.m_Offset = 0;
    }

    void Renderer::CreateBoxGeometry()
    {
        // Make the extents .5 and then offset them from the center of our shape to create a 1x1x1 cube with origin at the center
        glm::vec4 extents = { 0.5f, 0.5f, 0.5f, 1.f };

        // Unique vert positions of the box
        glm::vec4 boxCorners[] =
        {
            { -extents.x,  extents.y,  extents.z, extents.w },  	// V0 = -0.5,  0.5,  0.5
            { -extents.x,  extents.y, -extents.z, extents.w },		// V1 = -0.5,  0.5, -0.5
            {  extents.x,  extents.y, -extents.z, extents.w },		// V2 =  0.5,  0.5, -0.5
            {  extents.x,  extents.y,  extents.z, extents.w },		// V3 =  0.5,  0.5,  0.5
            { -extents.x, -extents.y,  extents.z, extents.w },		// V4 = -0.5, -0.5,  0.5
            { -extents.x, -extents.y, -extents.z, extents.w },		// V5 = -0.5, -0.5, -0.5
            {  extents.x, -extents.y, -extents.z, extents.w },		// V6 =  0.5, -0.5, -0.5
            {  extents.x, -extents.y,  extents.z, extents.w }		// V7 =  0.5, -0.5,  0.5
        };

        // Organized by face
        int boxIndices[] =
        {
            // +-x
            0,1,4,	4,1,5,
            2,3,6,	6,3,7,

            // +-y
            1,0,2,	2,0,3,
            4,5,6,	4,6,7,

            // +-z
            2,5,1,	2,6,5,
            3,0,4,	3,4,7
        };

        const int kVertsPerTri = 3;
        const int kTrisPerFace = 2;
        const int kFaces = 6;
        const int kVertCount = kVertsPerTri * kTrisPerFace * kFaces;

        Vertex boxVerts[kVertCount];

        // Duplicate verts so that we can caluclate proper face normals and get correct shading
        for (size_t i = 0; i < kVertCount; i++)
        {
            boxVerts[i].m_Position = boxCorners[boxIndices[i]];
        }

        // Calculate surface normals
        // Iterate through all our tris
        for (size_t i = 0; i < kVertCount; i += 3) 
        {
            // Get verts of current tri via indices
            glm::vec3 v0 = boxVerts[i + 0].m_Position;
            glm::vec3 v1 = boxVerts[i + 1].m_Position;
            glm::vec3 v2 = boxVerts[i + 2].m_Position;

            // Calculate edges
            glm::vec3 edge0 = v1 - v0;
            glm::vec3 edge1 = v2 - v0;

            // Calculate cross product
            glm::vec3 crossPrd = glm::normalize(glm::cross(edge0, edge1));
           
            // Normalise
            crossPrd = glm::normalize(crossPrd);

            // Set normals
            boxVerts[i + 0].m_Normal = crossPrd;
            boxVerts[i + 1].m_Normal = crossPrd;
            boxVerts[i + 2].m_Normal = crossPrd;
        }

        BufferDesc boxVBDesc
        {
            .m_Data = boxVerts,
            .m_SizeInBytes = sizeof(boxVerts),
            .m_Type = eBufferType::kVertex,
            .m_CPUVisible = false
        };

        s_BoxGeoBuffer = new GeometryBuffer();
        s_BoxGeoBuffer->vbView.m_Buffer = s_RendererBackend->CreateBuffer(boxVBDesc);
        s_BoxGeoBuffer->vbView.m_Buffer->Upload(boxVBDesc.m_Data, boxVBDesc.m_SizeInBytes);
        s_BoxGeoBuffer->vbView.m_Stride = sizeof(Vertex);
        s_BoxGeoBuffer->vbView.m_Offset = 0;

        // Re-create a dummy IB, each triangle now has unique verts
        int sequentialIndices[] =
        {
            0,1,2,	    3,4,5,
            6,7,8,	    9,10,11,

            12,13,14,   15,16,17,
            18,19,20,	21,22,23,

            24,25,26,	27,28,29,
            30,31,32,	33,34,35
        };

        BufferDesc boxIBDesc
        {
            .m_Data = sequentialIndices,
            .m_SizeInBytes = sizeof(sequentialIndices),
            .m_Type = eBufferType::kIndex,
            .m_CPUVisible = false
        };

        s_BoxGeoBuffer->ibView.m_Buffer = s_RendererBackend->CreateBuffer(boxIBDesc);
        s_BoxGeoBuffer->ibView.m_Buffer->Upload(boxIBDesc.m_Data, boxIBDesc.m_SizeInBytes);
        s_BoxGeoBuffer->ibView.m_Count = kVertCount;
        s_BoxGeoBuffer->ibView.m_IndexSize = sizeof(unsigned int);
        s_BoxGeoBuffer->ibView.m_Offset = 0;
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

        VertexAttribute aNormal = { eShaderSemantic::kNormal, eVertexAttributeFormat::kFloat3 };

        // Construct a layout with these attributes, offset and stride will be calculated internally
        VertexLayout defaultLayout({ aPos, aNormal });

        // Grab shader library and register shaders or grab handle in the case that they've already been registered (not the case here, but could be when called later!)
        ShaderLibrary& shaders = ShaderLibrary::Get();
        ShaderDesc vsDesc
        {
            .m_Name = "FlatColourVS",       // No extensions, ideally we have identical shaders for both GLSL and HLSL. Let the renderer API figure out which one it needs to loads
            .m_ShaderStage = eShaderStage::kVertex
        };
        ShaderHandle vsHandle = shaders.Register("FlatColourVS", vsDesc);

        ShaderDesc psDesc
        {
            .m_Name = "FlatColourPS",
            .m_ShaderStage = eShaderStage::kPixel
        };
        ShaderHandle psHandle = shaders.Register("FlatColourPS", psDesc);

        PipelineDesc pipelineDesc =
        {
            .m_VertexShader = vsHandle,
            .m_PixelShader = psHandle,
            .m_Layout = defaultLayout
        };

        // Register pipeline and store handle for use in material creation
        PipelineLibrary& pipelines = PipelineLibrary::Get();
        PipelineHandle solidColourPipeline = pipelines.Register(s_FlatColourPipeName, pipelineDesc);

        s_RendererBackend->CreatePipeline(pipelineDesc, solidColourPipeline);

        // Create and register the materials with this pipeline handle
        uint32_t materialStride = sizeof(FlatColourMaterialData);
        s_FlatColourMat = new Aether::Material(solidColourPipeline, materialStride, eMaterialType::kSolidColour);
       
        MaterialLibrary& materials = MaterialLibrary::Get();
        materials.Register(eMaterialType::kSolidColour, s_FlatColourMat);

        // Make sure our buffer for GPU material data is created too
        BufferDesc matBufDesc
        {
            //.m_Data = nullptr, // empty for now, will be uploaded per material instance
            .m_SizeInBytes = kMaxMaterialInstances * materialStride,
            .m_StructStride = materialStride,
            .m_Type = eBufferType::kStructuredStorage,
            .m_CPUVisible = true
        };

        s_FlatColourMaterialDataGPU = s_RendererBackend->CreateBuffer(matBufDesc);

        // Repeat for as many pipelines as we want!
        //

        ShaderHandle blinnPhongVS = shaders.Register("LitColourVS", { "LitColourVS", eShaderStage::kVertex });
        ShaderHandle blinnPhongPS = shaders.Register("LitColourPS", { "LitColourPS", eShaderStage::kPixel });
        PipelineDesc blinnPhongDesc = { blinnPhongVS, blinnPhongPS, defaultLayout };

        PipelineHandle blinnPhongHandle = pipelines.Register(s_LitColourPipeName, blinnPhongDesc);

        uint32_t bpMatStride = sizeof(LitColourMaterialData);
        s_LitColourMat = new Aether::Material(blinnPhongHandle, bpMatStride, eMaterialType::kBlinnPhong);
        materials.Register(eMaterialType::kBlinnPhong, s_LitColourMat);
        s_RendererBackend->CreatePipeline(blinnPhongDesc, blinnPhongHandle);
        BufferDesc bpMatBufDesc
        {
            //.m_Data = nullptr, // empty for now, will be uploaded per material instance
            .m_SizeInBytes = kMaxMaterialInstances * bpMatStride,
            .m_StructStride = bpMatStride,
            .m_Type = eBufferType::kStructuredStorage,
            .m_CPUVisible = true
        };

        s_LitColourMaterialDataGPU = s_RendererBackend->CreateBuffer(bpMatBufDesc);
    }
}