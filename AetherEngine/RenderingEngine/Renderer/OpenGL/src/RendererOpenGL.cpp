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
	return ar;
}

void Aether::RendererOpenGL::Render()
{
}

void Aether::RendererOpenGL::Terminate()
{
}
