#pragma once
//===============================================================================
// desc: Base class to handle GPU buffers across rendering APIs, modelled after modern APIs like DX12 and Vulkan, but still loose enough to work with OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	enum class eBufferType : uint16_t
	{
		kNone = 0,
		kVertex = 1 << 0,
		kIndex = 1 << 1,
		kUniform = 1 << 2,
		kStorage = 1 << 3,
		kTransferSrc = 1 << 4,
		kTransferDst = 1 << 5
	};

	struct BufferDesc
	{
		size_t      m_SizeInBytes;
		eBufferType m_Type;
		bool        m_CPUVisible;   // Staging / dynamic buffers that we want to read back from
	};

	class AETHER_API Buffer
	{
	public:
		virtual ~Buffer() = default;

		virtual size_t GetSize() const = 0;
		virtual eBufferType GetType() const = 0;

		// Explicit data upload that supports all types of buffers
		virtual void Upload(const void* data, size_t size, size_t offset = 0) = 0;
	};

	// Different kinds of views into the abstract buffer that we may want

	struct VertexBufferView
	{
		Buffer*		m_Buffer;
		uint32_t	m_Stride;
		size_t		m_Offset;
	};

	struct IndexBufferView
	{
		Buffer*		m_Buffer;
		uint32_t	m_IndexSize; // 16 or 32 bit (currently only 32 bit supported)
		uint32_t	m_Count;
		size_t		m_Offset;
	};
}