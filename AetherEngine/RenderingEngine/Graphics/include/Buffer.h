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
		kConstant,
		kUniform, 
		kStorage, 
		kTransferSrc,
		kTransferDst
	};

	struct AETHER_API BufferDesc
	{
		const void*	m_Data = nullptr;	// Optional paramater to upload data initially, or leave it for batch uploading optimisation
		size_t      m_SizeInBytes;
		eBufferType m_Type;
		bool        m_CPUVisible;   // Staging / dynamic buffers that we want to read back from
	};

	class AETHER_API Buffer
	{
	public:
		Buffer (const BufferDesc& desc)
			: m_Desc(desc) {}

		virtual ~Buffer() = default;

		virtual size_t GetSize() const = 0;
		virtual eBufferType GetType() const = 0;

		// Explicit data upload that supports all types of buffers
		virtual void Upload(const void* data, size_t size, size_t offset = 0) = 0;
		const BufferDesc& GetDesc() const { return m_Desc; }

	protected:
		BufferDesc m_Desc;
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