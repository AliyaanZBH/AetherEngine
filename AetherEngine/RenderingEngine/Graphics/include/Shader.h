#pragma once
//===============================================================================
// desc: A small utility class to handle shaders across rendering APIs (GLSL & HLSL)
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	// Handle that represents and identifies shaders. Used as an index into our library
	using ShaderHandle = uint32_t;

	enum class eShaderStage
	{
		kVertex,
		kPixel,
		kCompute
	};

	enum class eShaderSemantic
	{
		kPosition,
		kColour,
		kNormal,
		kTexCoord0,
		kTexCoord1,
		kTangent,
		kBitangent
	};

	struct ShaderDesc
	{
		std::string m_Name;		// Logical shader file name, full path will be derived later in respective renderer backend
		eShaderStage m_ShaderStage;
	};

	// Contains definitions for named shaders
	class AETHER_API ShaderLibrary
	{
	public:
		ShaderHandle Register(const std::string& name, const ShaderDesc& desc);

		ShaderHandle GetHandle(const std::string& name) const;
		const ShaderDesc& GetDesc(ShaderHandle handle) const;

		// New funky and apparently thread safe way to do a singleton I found
		static ShaderLibrary& Get();

	private:
		// Cache of our shaders and their handles
		std::unordered_map<std::string, ShaderHandle> m_ShaderCache;
		// Container for descriptions
		std::vector<ShaderDesc> m_Descs;
	};

	// Base class that will get implemented by each rendering API
	class AETHER_API Shader
	{
	public:
		virtual ~Shader() = default;
	};
}