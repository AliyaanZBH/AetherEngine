//===============================================================================
// desc: OpenGL pipeline object definition
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "PipelineOpenGL.h"
//===============================================================================

namespace Aether
{
	PipelineOpenGL::PipelineOpenGL(ShaderOpenGL* shaderProgram)
		: m_ShaderProgram(shaderProgram) {}

	void PipelineOpenGL::BindShader()
	{
		m_ShaderProgram->Bind();
	}
}
