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

	// Glad returns 1 on success, we use 0
	return ar - 1;
}

void Aether::RendererOpenGL::Render()
{
}

void Aether::RendererOpenGL::Terminate()
{
}
