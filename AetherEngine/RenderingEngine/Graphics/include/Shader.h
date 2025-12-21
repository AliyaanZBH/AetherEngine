#pragma once
//===============================================================================
// desc: A small utility class to handle shaders across rendering APIs (GLSL & HLSL)
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include <IRenderer.h>
//===============================================================================

namespace Aether
{
	class AETHER_API Shader
	{
	public:
		Shader(const std::string& vertSrc, const std::string& fragSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;
	private:
		eRenderAPI m_CurrentRendererAPI;
		GLuint m_GLProgram;
	};
}