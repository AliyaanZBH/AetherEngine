#pragma once
//===============================================================================
// desc: Base class to handle geometry buffers across rendering APIs
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	class AETHER_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		// Instead of a standard constructor, use a create function so that we can create any of the derived API-specific types without needing to know about them
		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class AETHER_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t size);

	};
}