#pragma once
//===============================================================================
// desc: A small utility class to handle pipeline creation across rendering APIs (GLSL & HLSL)
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Shader.h"
#include "Vertex.h"
//===============================================================================

namespace Aether
{
    using PipelineHandle = uint32_t;

    // High-level API-agnostic description of a pipeline
	struct PipelineDesc
	{
        ShaderHandle m_VertexShader;
        ShaderHandle m_PixelShader;
        VertexLayout m_Layout;

        //RasterState Raster;
        //DepthStencilState Depth;
        //BlendState Blend;
	};

	// Contains high-level definitions for named pipelines and stores handles to them to ensure backends don't create duplicates
	class AETHER_API PipelineLibrary
	{
	public:
		PipelineHandle Register(const std::string& name, const PipelineDesc& desc);

		PipelineHandle GetHandle(const std::string& name) const;
		const PipelineDesc& GetDesc(PipelineHandle handle) const;

		// New funky and apparently thread safe way to do a singleton I found
		static PipelineLibrary& Get();

	private:
		// Cache of our pipes and their handles
		std::unordered_map<std::string, PipelineHandle> m_PipelineCache;
		// Container for descriptions
		std::vector<PipelineDesc> m_Descs;
	};

	// Base class that will get implemented by each rendering API
	class AETHER_API Pipeline
	{
	public:
		virtual ~Pipeline() = default;
	};
}