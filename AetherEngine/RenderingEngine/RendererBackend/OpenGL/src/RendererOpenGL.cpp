//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererOpenGL.h"
#include "BufferOpenGL.h"
#include "DrawCommand.h"
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
		case eShaderSemantic::kColour:    return 1;
		case eShaderSemantic::kNormal:    return 2;
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

		// Create geometry itself - centered tri for now
		Vertex verts[]
		{
			{ { -0.5f,	-0.5f,	0.f, 1.f},	{1.f, 0.f, 0.f, 1.f} },
			{ {  0.5f,	-0.5f,	0.f, 1.f},	{0.f, 1.f, 0.f, 1.f} },
			{ {  0.f,	 0.5f,	0.f, 1.f},	{0.f, 0.f, 1.f, 1.f} }
		};

		// Create vertex buffer here with our fancy new buffer desc
		BufferDesc vbDesc =
		{
			.m_Data = verts,
			.m_SizeInBytes = sizeof(verts),
			.m_Type = eBufferType::kVertex,
			.m_CPUVisible = true
		};

		m_VertexBuffer = static_cast<BufferOpenGL*>(CreateBuffer(vbDesc));

		// Memory has been allocated, now upload data to it
		m_VertexBuffer->Upload(verts, vbDesc.m_SizeInBytes);

		// Create indices and repeat
		unsigned int indices[3] = { 0, 1, 2 };
		BufferDesc ibDesc =
		{
			.m_Data = indices,	// We can skip this (as we did with verts) and it will default to nullptr, allowing us to manually upload data later. Supplying this causes an immediate upload
			.m_SizeInBytes = sizeof(indices),
			.m_Type = eBufferType::kIndex,
			.m_CPUVisible = true
		};
		m_IndexBuffer = static_cast<BufferOpenGL*>(CreateBuffer(ibDesc));
		m_IndexBuffer->Upload(indices, ibDesc.m_SizeInBytes);

		return ar;
	}

	ShaderOpenGL* RendererOpenGL::LoadShader(ShaderHandle vertHandle, ShaderHandle fragHandle)
	{
		// See if this shader was already compiled, return it if so. 
		// Only check for the vert shader, OpenGL shaders are made in pairs so if the vertex shader can't be found, then neither will the frag
		auto it = m_ShaderCache.find(vertHandle);
		if (it != m_ShaderCache.end())
			return it->second;

		// Doesn't exist yet, let's build it
		const ShaderDesc& vsDesc = ShaderLibrary::Get().GetDesc(vertHandle);
		const ShaderDesc& psDesc = ShaderLibrary::Get().GetDesc(fragHandle);

		// Push into vector
		const std::vector<ShaderDesc> shaderDescs = { vsDesc, psDesc };

		ShaderOpenGL* shader = new ShaderOpenGL(shaderDescs);

		// Register shaders in local OpenGL cache
		m_ShaderCache[vertHandle] = shader;
		m_ShaderCache[fragHandle] = shader;

		return shader;
	}

	void RendererOpenGL::CreatePipeline(const PipelineDesc& desc)
	{
		// Grab or create shaders for this openGL "Pipeline"
		//
		
		// OpenGL requires that both vert and frag shaders are compiled together into a SINGLE GLprogram object
		m_DefaultShader = LoadShader(desc.m_VertexShader, desc.m_PixelShader);

		// Bind those bois
		m_DefaultShader->Bind();

		// Create VAO object to define our input layout
		glCreateVertexArrays(1, &m_VertexAttributeArray);

		// Propogate pipeline layout to get vertex attributes
		for (VertexAttribute attrib : desc.m_Layout.m_Attributes)
		{
			// Derive location from semantic name (position is 0, colour 1, etc.)
			uint32_t location = ToGLLocation(attrib.m_Name);
			// Define the attribute in the OpenGL format
			glVertexArrayAttribFormat(
				m_VertexAttributeArray,
				location,											// attrib index
				VertexAttributeComponentCount(attrib.m_Format),		// Size  in OpenGL land actually means `component count`. So we want 4 instead of 16
				ToGLFormat(attrib.m_Format),						// GL enum that matches our format (usually GL_FLOAT)
				GL_FALSE,											// Force false on normalisation for now
				attrib.m_Offset										// offset within vertex struct ( e.g. colour would be 16 bytes offset as there are 16 bytes of position data first)
			);

			// Set the binding location to be used in the shaders. In my engine today, each vertex attribute location maps to a single binding slot, because all attributes come from the same interleaved buffer. 
			glVertexArrayAttribBinding(m_VertexAttributeArray, location, 0);

			// Finally, enable the attribute in the vertex array
			glEnableVertexArrayAttrib(m_VertexAttributeArray, location);
		}

		// Store uniform locations
		m_TransformLocation = glGetUniformLocation(m_DefaultShader->GetProgram(), "u_Transform");
		m_SolidColourLocation = glGetUniformLocation(m_DefaultShader->GetProgram(), "u_SolidColour");
		m_DynamicColourLocation = glGetUniformLocation(m_DefaultShader->GetProgram(), "u_DynamicColour");

	}

	void RendererOpenGL::Render()
	{
		// Uniform setup for varying colours!
		float deltaTime = glfwGetTime();
		float varyingVal = (sin(deltaTime) / 2.0f) + 0.15f;

		glUniform4f(m_DynamicColourLocation, varyingVal, varyingVal, varyingVal, 1.0f);
		
		// No transform on this guy
		//glUniformMatrix4fv(m_TransformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));

		// Bind and draw our geo!
		glBindVertexArray(m_VertexAttributeArray);
		
		glVertexArrayVertexBuffer(
			m_VertexAttributeArray,
			0,   // same index used when binding attributes for the VAO
			m_VertexBuffer->GetHandle(),
			0,
			sizeof(Vertex)
		);

		// Bind index buffer
		glVertexArrayElementBuffer(m_VertexAttributeArray, m_IndexBuffer->GetHandle());
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
	}

	void RendererOpenGL::Submit(const DrawCommand& cmd, ConstantBufferView* cbv)
	{
		BindConstantBuffer(cbv);

		// OpenGL is immediate mode so we can render immediately
		Render(cmd.m_VBV, cmd.m_IBV);
	}

	void RendererOpenGL::Render(VertexBufferView* vbv, IndexBufferView* ibv)
	{
		// Need to rebind vertex array as it is still pointing at the old set of geometry!
		glBindVertexArray(m_VertexAttributeArray);
		
		// Only want the dynamic colour on the other bit of geo, so reset the value to 0 here for the main background
		glUniform4f(m_DynamicColourLocation, 0.f, 0.f, 0.f, 1.0f);

		glVertexArrayVertexBuffer(
			m_VertexAttributeArray,
			0,   // same index used when binding attributes for the VAO
			static_cast<BufferOpenGL*>(vbv->m_Buffer)->GetHandle(),
			vbv->m_Offset,
			vbv->m_Stride
		);

		glVertexArrayElementBuffer(m_VertexAttributeArray, static_cast<BufferOpenGL*>(ibv->m_Buffer)->GetHandle());
		glDrawElements(GL_TRIANGLES, ibv->m_Count, GL_UNSIGNED_INT, nullptr);

	}
	
	void RendererOpenGL::Present()
	{
		glfwSwapBuffers(m_pWindow);
	}
	
	void RendererOpenGL::ClearFrame()
	{
		glClearColor(0.2f, 0.7f, 0.9f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	void RendererOpenGL::Terminate()
	{
		delete m_VertexBuffer;
		delete m_IndexBuffer;
	}

	Buffer* RendererOpenGL::CreateBuffer(const BufferDesc& desc)
	{
		return new BufferOpenGL(desc);
	}

	void RendererOpenGL::BindFrameConstants(const ConstantBufferView* cbv)
	{
		BindConstantBuffer(cbv);
	}

	void RendererOpenGL::BindConstantBuffer(const ConstantBufferView* cbv)
	{
		const BufferOpenGL* glBuf =
			static_cast<const BufferOpenGL*>(cbv->m_Buffer);

		glBindBufferBase(
			GL_UNIFORM_BUFFER,
			cbv->m_Slot,
			glBuf->GetHandle()
		);
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