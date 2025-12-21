#pragma once
//===============================================================================
// desc: Implmentation of geometry buffers for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Buffer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API VertexBufferOpenGL : public VertexBuffer
	{
	public:
		VertexBufferOpenGL(float* vertices, uint32_t size);
		~VertexBufferOpenGL();
		
		void Bind() override;
		void Unbind() override;
	private:
		uint32_t m_Buffer = 0u;
	};

	class AETHER_API IndexBufferOpenGL : public IndexBuffer
	{
	public:
		IndexBufferOpenGL(uint32_t* indices, uint32_t size);
		~IndexBufferOpenGL();

		void Bind() override;
		void Unbind() override;
	private:
		uint32_t m_Buffer = 0u;

	};
}