//===============================================================================
// desc: A small utility class to handle pipeline creation across rendering APIs (GLSL & HLSL)
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Pipeline.h"
#include "AetherUtils.h"
//===============================================================================
namespace Aether
{
	PipelineHandle PipelineLibrary::Register(const std::string& name, const PipelineDesc& desc)
	{
		// Check if it's already been registered, don't want to do this twice
		auto it = m_PipelineCache.find(name);
		if (it != m_PipelineCache.end())
			return it->second;

		// Register a new pipeline, grab the current size of the library to ensure we don't overwrite an existing handle (these are created incrementally currently)
		PipelineHandle handle = m_Descs.size();

		// Insert into cache and push back the description
		m_PipelineCache[name] = handle;
		m_Descs.push_back(desc);

		return handle;
	}

	PipelineHandle PipelineLibrary::GetHandle(const std::string& name) const
	{
		auto it = m_PipelineCache.find(name);
		AETHER_RESULT ar = !(it != m_PipelineCache.end());	// Not here as we use 0 for success, while this maps to false as a bool.
		AETHER_ASSERT(ar, "Pipeline not registered");
		return it->second;
	}

	const PipelineDesc& PipelineLibrary::GetDesc(PipelineHandle handle) const
	{
		AETHER_RESULT ar = !(handle < m_Descs.size());
		AETHER_ASSERT(ar, "Invalid pipeline handle");
		return m_Descs[handle];
	}

	PipelineLibrary& PipelineLibrary::Get()
	{
		static PipelineLibrary instance;
		return instance;
	}
}
