#pragma once
//===============================================================================
// desc: Shader class implementation for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Shader.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ShaderOpenGL final : public Shader
	{
	public:
		ShaderOpenGL(const std::vector<ShaderDesc>& shaderDescs);
		
		GLenum ShaderStageToGLSLCompilerEnum(eShaderStage stage);

		const GLuint GetProgram() const { return m_GLProgram; }
		void Bind();
		void Unbind();
	private:
		GLuint m_GLProgram;
	};
}