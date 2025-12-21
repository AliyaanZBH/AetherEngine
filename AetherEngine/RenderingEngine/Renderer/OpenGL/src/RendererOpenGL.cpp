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
	
		// Glad returns 1 on success, we use 0
		return ar - 1;
	}
	
	void RendererOpenGL::Render()
	{} // Currently not rendering anything!
	
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