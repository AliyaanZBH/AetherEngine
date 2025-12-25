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

		void Bind();
		void Unbind();
	private:
		GLuint m_GLProgram;
	};
}