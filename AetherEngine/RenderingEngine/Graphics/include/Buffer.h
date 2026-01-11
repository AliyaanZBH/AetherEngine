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
		kNone,
		kVertex,
		kIndex ,
		kConstantPerFrame,
		kConstantPerDraw,
		kStructured,
		kStorage, 
		kTransferSrc,
		kTransferDst
	};

	struct AETHER_API BufferDesc
	{
		const void*	m_Data = nullptr;		// Optional paramater to upload data initially, or leave it for batch uploading optimisation
		size_t      m_SizeInBytes;
		size_t      m_StructStride = 0;		// TMP: Required for structured buffers but I think this breaks abstraction somewhat.
		eBufferType m_Type;
		bool        m_CPUVisible;			// Staging / dynamic buffers that we want to read back from
	};

	class AETHER_API Buffer
	{
	public:
		Buffer (const BufferDesc& desc)
			: m_Desc(desc) {}

		virtual ~Buffer() = default;

		// Explicit data upload that supports all types of buffers
		virtual void Upload(const void* data, size_t size, size_t offset = 0) = 0;

		const BufferDesc& GetDesc() const { return m_Desc; }
		size_t GetSize() const { return m_Desc.m_SizeInBytes; }
		eBufferType GetType() const { return m_Desc.m_Type; }

	protected:
		BufferDesc m_Desc;
	};

	// Different kinds of views into the abstract buffer that we may want

	struct AETHER_API VertexBufferView
	{
		Buffer*		m_Buffer = nullptr;
		uint32_t	m_Stride = 0u;
		size_t		m_Offset = 0u;
	};

	struct AETHER_API IndexBufferView
	{
		Buffer*		m_Buffer = nullptr;
		uint32_t	m_IndexSize = 0u; // 16 or 32 bit (currently only 32 bit supported)
		uint32_t	m_Count = 0u;
		size_t		m_Offset = 0u;
	};


	struct AETHER_API ConstantBufferView
	{
		Buffer*		m_Buffer = nullptr;
		uint32_t	m_Size = 0u;
		uint32_t	m_Slot = 0u; // Which register is the CB getting bound to
	};

	struct AETHER_API GeometryBuffer
	{
		VertexBufferView vbView = {};
		IndexBufferView ibView = {};
	};
}
