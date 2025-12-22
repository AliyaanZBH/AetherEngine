//===============================================================================
// desc: A small utility class to handle shaders across rendering APIs (GLSL & HLSL)
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Shader.h"
#include "AetherUtils.h"
//===============================================================================
namespace Aether
{
	ShaderHandle ShaderLibrary::Register(const std::string& name, const ShaderDesc& desc)
	{
		// Check if it's already been registered, don't want to do this twice
		auto it = m_ShaderCache.find(name);
		if (it != m_ShaderCache.end())
			return it->second;

		// Register a new shader, grab the current size of the library to ensure we don't overwrite an existing handle (these are created incrementally currently)
		ShaderHandle handle = m_Descs.size();
		
		// Insert into cache and push back the description
		m_ShaderCache[name] = handle;
		m_Descs.push_back(desc);

		return handle;
	}

	ShaderHandle ShaderLibrary::GetHandle(const std::string& name) const
	{
		auto it = m_ShaderCache.find(name);
		AETHER_RESULT ar = !(it != m_ShaderCache.end());	// Not here as we use 0 for success, while this maps to false as a bool.
		AETHER_ASSERT(ar, "Shader not registered");
		return it->second;
	}

	const ShaderDesc& ShaderLibrary::GetDesc(ShaderHandle handle) const
	{
		AETHER_RESULT ar = !(handle < m_Descs.size());
		AETHER_ASSERT(ar, "Invalid shader handle");
		return m_Descs[handle];
	}

	ShaderLibrary& ShaderLibrary::Get()
	{
		static ShaderLibrary instance;
		return instance;
	}
}