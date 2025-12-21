//===============================================================================
// desc: Implmentation of geometry buffers for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "BufferOpenGL.h"
//===============================================================================

namespace Aether
{

	//
	// Vertex Buffer
	//

	VertexBufferOpenGL::VertexBufferOpenGL(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_Buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	}

	VertexBufferOpenGL::~VertexBufferOpenGL()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	void VertexBufferOpenGL::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
	}

	void VertexBufferOpenGL::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	//
	// Index Buffer
	//


	IndexBufferOpenGL::IndexBufferOpenGL(uint32_t* indices, uint32_t size)
	{
		glCreateBuffers(1, &m_Buffer);
		glBufferData(GL_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	IndexBufferOpenGL::~IndexBufferOpenGL()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	void IndexBufferOpenGL::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
	}

	void IndexBufferOpenGL::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}