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

		// Using namedbuffer for Direct State Access in order to avoid global mutable state and hidden state changes
		glNamedBufferData(m_Handle, desc.m_SizeInBytes, desc.m_Data, desc.m_CPUVisible ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);	// Ternary here to allow for static and dynamic draws based on the description we pass in!
	}

	BufferOpenGL::~BufferOpenGL()
	{
		glDeleteBuffers(1, &m_Handle);
	}

	void BufferOpenGL::Upload(const void* data, size_t size, size_t offset)
	{
		glNamedBufferSubData(m_Handle, offset, size, data);
	}

}