#pragma once
//===============================================================================
// desc: Implmentation of GPU buffers for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Buffer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API BufferOpenGL final : public Buffer
	{
	public:
		BufferOpenGL(const BufferDesc& desc);
		~BufferOpenGL();

		void Upload(const void* data, size_t size, size_t offset = 0) override;
		
		GLuint GetHandle() const { return m_Handle; }
		size_t GetSize() const override { return m_Desc.m_SizeInBytes; }
		eBufferType GetType() const override { return m_Desc.m_Type; }

	private:
		GLuint m_Handle = 0u;
	};
}