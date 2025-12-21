//===============================================================================
// desc: Base class to handle geometry buffers across rendering APIs
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Buffer.h"
#include "BufferOpenGL.h"
#include "CoreApp.h"
//===============================================================================

namespace Aether
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Application::Get().GetRenderAPI())
		{
			case eRenderAPI::kOpenGL:
			{
				return new VertexBufferOpenGL(vertices, size);
			}
			case eRenderAPI::kDX12:
			{
				break;
			}
			default:
			{
				AETHER_ASSERT(AETHER_FAIL, "Attempted to create buffer for non-existent render API!");
			}
		}
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		return nullptr;
	}
}