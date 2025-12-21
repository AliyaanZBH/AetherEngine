//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererOpenGL.h"
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
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		// Create geometry itself - centered tri for now
		float verts[3 * 3]
		{
			-0.5f,	-0.5f,	0.f,
			0.5f,	-0.5f,	0.f,
			0.f,	0.5f,	0.f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

		// Enable vert attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		// Create indices
		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


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
		m_Shader = new Shader(vertexSrc, fragSrc);
		//m_Shader = new Shader("VertexShader.glsl", "FragShader.glsl");
		// Bind that boy
		m_Shader->Bind();

		return ar;
	}
	
	void RendererOpenGL::Render()
	{
		// Bind and draw our geo!
		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
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