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
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
	};
}