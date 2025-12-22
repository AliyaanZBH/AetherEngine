//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererOpenGL.h"
#include "BufferOpenGL.h"
//===============================================================================

namespace Aether
{

	AETHER_RESULT RendererOpenGL::Initialize(IWindow& window)
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

		// Create vertex buffer and array
		glCreateVertexArrays(1, &m_VertexAttributeArray);
		// Attrib 0: Position
		glEnableVertexArrayAttrib(m_VertexAttributeArray, 0);
		glVertexArrayAttribFormat(
			m_VertexAttributeArray,
			0,                  // attrib index
			4,                  // vec4
			GL_FLOAT,
			GL_FALSE,
			0                   // offset within vertex
		);
		glVertexArrayAttribBinding(m_VertexAttributeArray, 0, 0);

		// Binding slot 0 defines stride
		glVertexArrayBindingDivisor(m_VertexAttributeArray, 0, 0);

		//glGenVertexArrays(1, &m_VertexArray);
		//glBindVertexArray(m_VertexAttributeArray);

		// Create geometry itself - centered tri for now
		float verts[4 * 3]
		{
			-0.5f,	-0.5f,	0.f, 1.f,
			0.5f,	-0.5f,	0.f, 1.f,
			0.f,	0.5f,	0.f, 1.f
		};

		// Create vertex buffer here with our fancy new buffer desc
		BufferDesc vbDesc =
		{
			.m_SizeInBytes = sizeof(verts),
			.m_Type = eBufferType::kVertex,
			.m_CPUVisible = true
		};

		m_VertexBuffer = static_cast<BufferOpenGL*>(CreateBuffer(vbDesc));

		// Memory has been allocated, now upload data to it
		m_VertexBuffer->Upload(verts, vbDesc.m_SizeInBytes);

		// Bind that boy
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer->GetHandle());

		// Enable vert attributes
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		//glEnableVertexAttribArray(0);

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
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer->GetHandle());


		// TMP: Paste simple shader source here for now
		std::string vertexSrc = R"(
			#version 430 core
			layout (location = 0) in vec3 a_Pos;
			
			out vec3 v_Position;

			void main()
			{
				v_Position = a_Pos;
			    gl_Position = vec4(a_Pos, 1.0);
			}
		)";

		std::string fragSrc = R"(
			#version 430 core
			layout (location = 0) out vec4 colour;
			
			in vec3 v_Position;

			void main()
			{
			    colour = vec4(v_Position * 0.5 + 0.65, 1.0);
			}
		)";

		// Create shader program by compiling and linking shader files ( or raw source as we have it currently)
		m_Shader = new ShaderOpenGL(vertexSrc, fragSrc);
		//m_Shader = new Shader("VertexShader.glsl", "FragShader.glsl");
		// Bind that boy
		m_Shader->Bind();

		return ar;
	}

	void RendererOpenGL::Render()
	{
		// Bind and draw our geo!
		glBindVertexArray(m_VertexAttributeArray);
		
		// Bind vertex buffer to binding slot 0
		glBindVertexBuffer(
			0,                                  // binding index
			m_VertexBuffer->GetHandle(),
			0,
			4 * sizeof(float)
		);

		// Bind index buffer (this IS VAO state, unavoidable)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer->GetHandle());
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
	}

	void RendererOpenGL::Render(VertexBufferView* vbv, IndexBufferView* ibv)
	{
		// Need to rebind vertex array as it is still pointing at the old set of geometry!
		glBindVertexArray(m_VertexAttributeArray);
		// Bind vertex buffer to binding slot 0
		glBindVertexBuffer(
			0,                                  // binding index
			static_cast<BufferOpenGL*>(vbv->m_Buffer)->GetHandle(),
			vbv->m_Offset,
			vbv->m_Stride
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<BufferOpenGL*>(ibv->m_Buffer)->GetHandle());
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
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

}