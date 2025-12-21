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
		ShaderOpenGL(const std::string& vertSrc, const std::string& fragSrc);

		void Bind() override;
		void Unbind() override;
	private:
		GLuint m_GLProgram;
	};
}