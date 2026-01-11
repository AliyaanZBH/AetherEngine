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

		void Upload(const void* data, size_t size, size_t offset = 0, bool overwrite = true) override;
		
		GLuint GetHandle() const { return m_Handle; }

	private:
		GLuint m_Handle = 0u;
	};
}