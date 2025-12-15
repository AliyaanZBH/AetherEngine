//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererOpenGL.h"
//===============================================================================

AETHER_RESULT Aether::RendererOpenGL::Initialize(IWindow& window)
{
	AETHER_RESULT ar = AETHER_OK;
	ar = gladLoadGL();

	m_pWindow = static_cast<GLFWwindow*>(window.GetNativeWindowHandle());
	// Glad returns 1 on success, we use 0
	return ar - 1;
}

void Aether::RendererOpenGL::Render()
{
	glfwSwapBuffers(m_pWindow);
}

void Aether::RendererOpenGL::ClearFrame()
{
	glClearColor(0.2f, 0.7f, 0.9f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Aether::RendererOpenGL::Terminate()
{
}
