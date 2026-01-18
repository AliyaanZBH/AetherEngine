//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererOpenGL.h"
#include "BufferOpenGL.h"
#include "DrawCommand.h"
#include "GraphicsCommon.h"
#include "Pipeline.h"
#include <glm/gtc/type_ptr.hpp>
//===============================================================================

namespace Aether
{
	// TMP
	uint32_t ToGLLocation(eShaderSemantic semantic)
	{
		switch (semantic)
		{
		case eShaderSemantic::kPosition:  return 0;
		case eShaderSemantic::kNormal:    return 1;
		case eShaderSemantic::kColour:    return 2;
		case eShaderSemantic::kTexCoord0: return 3;
		case eShaderSemantic::kTexCoord1: return 4;
		default:
			AETHER_ASSERT(false, "Unsupported GLSL semantic");
			return 0;
		}
	}

	GLenum ToGLFormat(eVertexAttributeFormat format)
	{
		switch (format)
		{
			case eVertexAttributeFormat::kFloat:  
			case eVertexAttributeFormat::kFloat2: 
			case eVertexAttributeFormat::kFloat3: 
			case eVertexAttributeFormat::kFloat4:   return GL_FLOAT;
		default:
			AETHER_ASSERT(false, "Unsupported format for OpenGL");
			return 0;
		}
	}

	AETHER_RESULT RendererOpenGL::Initialize(const IWindow& window)
	{
		AETHER_RESULT ar = AETHER_OK;
	
		m_pWindow = static_cast<GLFWwindow*>(window.GetNativeWindowHandle());
	
		// Set OpenGL context, ready for loading openGL properly
		glfwMakeContextCurrent(m_pWindow);
	
		// Actually load now via glad
		ar = gladLoadGL();
		// Glad returns 1 on success, we use 0 so decrement the result and we should be good
		ar--;
		AETHER_ASSERT(ar, "Failed to load openGL via glad");

		// Nice little bit of logging to see what renderer we're using (integrated vs hopefully dedicated!
		const char* vendorString = (const char*)glGetString(GL_VENDOR);
		const char* rendererString = (const char*)glGetString(GL_RENDERER);
		const char* versionString = (const char*)glGetString(GL_VERSION);
		AETHER_CORE_INFO("OpenGL Info:\n    Vendor: {0}\n    Device: {1}\n    GL Version & Driver: {2}", vendorString, rendererString, versionString);
		
		// Get depth testing turned on so we don't get weirdness (and also get those mad optimisations in too :D)
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// For wireframe testing!
		//glPolygonMode(GL_FRONT, GL_FILL);
		//glCullFace(GL_BACK);

		return ar;
	}

	ShaderOpenGL* RendererOpenGL::LoadShader(const ShaderHandle vertHandle, const ShaderHandle fragHandle)
	{
		// Doesn't exist yet, let's build it
		const ShaderDesc& vsDesc = ShaderLibrary::Get().GetDesc(vertHandle);
		const ShaderDesc& psDesc = ShaderLibrary::Get().GetDesc(fragHandle);

		// Push into vector
		const std::vector<ShaderDesc> shaderDescs = { vsDesc, psDesc };

		// Create and shader program
		return new ShaderOpenGL(shaderDescs);
	}

	void RendererOpenGL::CreatePipeline(const PipelineDesc& desc, const PipelineHandle handle)
	{
		// Grab or create shaders for this openGL "Pipeline"
		//
		
		// OpenGL requires that both vert and frag shaders are compiled together into a SINGLE GLprogram object
		ShaderOpenGL* shader = LoadShader(desc.m_VertexShader, desc.m_PixelShader);

		// Create pipeline in cache
		m_PipelineCache[handle] = new PipelineOpenGL(shader);

		// VAOs will be lazily created and cached as necessary
	}

	void RendererOpenGL::BindPipeline(const PipelineHandle handle)
	{
		// Bind pipeline and use linked shader program
		m_PipelineCache[handle]->BindShader();

		// Store this handle to be used when looking up VAOs
		m_CurrentPipeline = handle;
	}

	void RendererOpenGL::BindGlobalResources(Buffer* materialBuffer)
	{
		const BufferOpenGL* glBuf = static_cast<const BufferOpenGL*>(materialBuffer);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ShaderBindings::kMaterialSRV, glBuf->GetHandle());
	}

	void RendererOpenGL::Render()
	{
	}

	void RendererOpenGL::Submit(const DrawCommand& cmd, ConstantBufferView* cbv)
	{
		BindConstantBuffer(cbv);

		// OpenGL is immediate mode so we can render immediately
		Render(cmd.m_VBV, cmd.m_IBV);
	}

	void RendererOpenGL::Render(VertexBufferView* vbv, IndexBufferView* ibv)
	{
		GLuint vbHandle = static_cast<const BufferOpenGL*>(vbv->m_Buffer)->GetHandle();
		GLuint ibHandle = static_cast<const BufferOpenGL*>(ibv->m_Buffer)->GetHandle();

		// Creates VAO for unique pair of geometry and pipeline once, looks them up from cache on subsequent 
		GLuint vao = LoadVAO(vbHandle, ibHandle);
		glBindVertexArray(vao);
		// Bind geometry buffers
		glVertexArrayVertexBuffer(vao, 0, vbHandle, 0, vbv->m_Stride);

		glVertexArrayElementBuffer(vao, ibHandle);
		glDrawElements(GL_TRIANGLES, ibv->m_Count, GL_UNSIGNED_INT, nullptr);
	}
	
	void RendererOpenGL::Present()
	{
		glfwSwapBuffers(m_pWindow);
	}
	
	void RendererOpenGL::ClearFrame()
	{
		glClearColor(0.2f, 0.7f, 0.9f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	void RendererOpenGL::Terminate()
	{

	}

	Buffer* RendererOpenGL::CreateBuffer(const BufferDesc& desc)
	{
		return new BufferOpenGL(desc);
	}

	void RendererOpenGL::BindFrameConstants(const ConstantBufferView* cbv)
	{
		BindConstantBuffer(cbv);
	}

	void RendererOpenGL::FinalizeUploads()
	{
		// TODO: Pre-build VAOs for known geometry-pipeline pairs at this point
		m_VAOCache.clear();
	}

	void RendererOpenGL::BindConstantBuffer(const ConstantBufferView* cbv)
	{
		const BufferOpenGL* glBuf = static_cast<const BufferOpenGL*>(cbv->m_Buffer);

		glBindBufferBase(GL_UNIFORM_BUFFER, cbv->m_Slot, glBuf->GetHandle());
	}

	GLuint RendererOpenGL::LoadVAO(const GLuint vbHandle, const GLuint ibHandle)
	{
		// Create a key with this pair of geometry buffers and pipeline
		//VAOKey key{ vbHandle, ibHandle, m_CurrentPipeline };

		// Use key to check if we've already created this VAO, return it if so
		//auto it = m_VAOCache.find(key);
		auto it = m_VAOCache.find(m_CurrentPipeline);
		if (it != m_VAOCache.end())
			return it->second;

		// Otherwise, continue creating this VAO

		// Create VAO object to define our input layout
		GLuint vertexAttributeArray;
		glCreateVertexArrays(1, &vertexAttributeArray);

		// Get desc of current pipe
		const PipelineDesc& desc = PipelineLibrary::Get().GetDesc(m_CurrentPipeline);

		// Propogate pipeline layout to get vertex attributes
		for (const VertexAttribute& attrib : desc.m_Layout.m_Attributes)
		{
			// Derive location from semantic name (position is 0, colour 1, etc.)
			uint32_t location = ToGLLocation(attrib.m_Name);
			// Define the attribute in the OpenGL format
			glVertexArrayAttribFormat(
				vertexAttributeArray,
				location,											// attrib index
				VertexAttributeComponentCount(attrib.m_Format),		// Size  in OpenGL land actually means `component count`. So we want 4 instead of 16
				ToGLFormat(attrib.m_Format),						// GL enum that matches our format (usually GL_FLOAT)
				GL_FALSE,											// Force false on normalisation for now
				attrib.m_Offset										// offset within vertex struct ( e.g. colour would be 16 bytes offset as there are 16 bytes of position data first)
			);

			// Set the binding location to be used in the shaders. In my engine today, each vertex attribute location maps to a single binding slot, because all attributes come from the same interleaved buffer. 
			glVertexArrayAttribBinding(vertexAttributeArray, location, 0);

			// Finally, enable the attribute in the vertex array
			glEnableVertexArrayAttrib(vertexAttributeArray, location);
		}

		// Bind geometry buffers
		//glVertexArrayVertexBuffer(
		//	vertexAttributeArray,
		//	0,
		//	vbHandle,
		//	0,
		//	desc.m_Layout.m_Stride
		//);
		//
		//glVertexArrayElementBuffer(vertexAttributeArray, ibHandle);

		// Cache VAO for reuse
		//m_VAOCache[key] = vertexAttributeArray;
		m_VAOCache[m_CurrentPipeline] = vertexAttributeArray;
		return vertexAttributeArray;
	}

	
	void RendererOpenGL::InitImGui()
	{
		ImGui_ImplOpenGL3_Init("#version 410");
	}
	
	void RendererOpenGL::BeginImGuiRender()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
	
	void RendererOpenGL::EndImGuiRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

}