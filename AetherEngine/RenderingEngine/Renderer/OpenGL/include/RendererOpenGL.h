#pragma once
//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IRenderer.h"
//===============================================================================
namespace Aether
{
	class RendererOpenGL : public IRenderer
	{
		// Main start up function
		AETHER_RESULT Initialize(IWindow& window) override;
		void Render() override;
		void Terminate() override;
	};
};