//===============================================================================
// desc: Implmentation of GPU buffers for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "BufferOpenGL.h"
//===============================================================================

namespace Aether
{
	BufferOpenGL::BufferOpenGL(const BufferDesc& desc)
		: Buffer(desc)
	{
		glCreateBuffers(1, &m_Handle);

		GLbitfield storageFlags = 0;

		if (desc.m_CPUVisible)
		{
			storageFlags |= GL_DYNAMIC_STORAGE_BIT;
		}

		glNamedBufferStorage(m_Handle, desc.m_SizeInBytes, desc.m_Data, storageFlags);
		
	}

	BufferOpenGL::~BufferOpenGL()
	{
		glDeleteBuffers(1, &m_Handle);
	}

	void BufferOpenGL::Upload(const void* data, size_t size, size_t offset, bool overwrite)
	{
		glNamedBufferSubData(m_Handle, offset, size, data);
	}
}