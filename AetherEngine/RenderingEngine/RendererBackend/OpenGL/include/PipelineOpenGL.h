#pragma once
//===============================================================================
// desc: OpenGL pipeline object definition
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Pipeline.h"
#include "ShaderOpenGL.h"
//===============================================================================

namespace Aether
{
	class Buffer;

	class AETHER_API PipelineOpenGL final : public Pipeline
	{
	public:

		explicit PipelineOpenGL(ShaderOpenGL* shaderProgram);

		void BindShader();
		ShaderOpenGL* GetShader() const { return m_ShaderProgram; }
	private:
		ShaderOpenGL* m_ShaderProgram;
	};

	// Key to look up our VAOs
	struct VAOKey
	{
		const GLuint			m_VertexBuffer;
		const GLuint			m_IndexBuffer;
		const PipelineHandle	m_Pipeline;

		// Equality operator to check we aren't duplicating VAOs, also useful inside unordered_map for key comparisons
		bool operator==(const VAOKey& other) const
		{
			return m_Pipeline == other.m_Pipeline && m_VertexBuffer == other.m_VertexBuffer && m_IndexBuffer == other.m_IndexBuffer;
		}
	};

	// We also need a hasher cos STL. I don't really understand hashing so here is one I copy-pasted.
	// TODO: Do some research and see if this is okay or if we need something better
	struct VAOKeyHasher
	{
		size_t operator()(const VAOKey& key) const noexcept
		{
			size_t h1= std::hash<uint64_t>{}(key.m_Pipeline);
			size_t h2 = std::hash<GLuint>{}(key.m_VertexBuffer);
			return h1 ^ (h2 << 1);
		}
	};
}
