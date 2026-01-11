#pragma once
//===============================================================================
// desc: Base class for a material in Aether, which defines a shading pipeline. There should only be a few of these, with many instances of a given material populating the scene.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	using PipelineHandle = uint32_t;
	using MaterialHandle = uint32_t;

	enum class eMaterialType {kSolidColour, kTextured, kPBR};

	constexpr uint32_t kMaxMaterialInstances = 512;

	class AETHER_API Material
	{
	public:
		Material(PipelineHandle pipeline, uint32_t dataStride, eMaterialType type)
			: m_Pipeline(pipeline), m_DataStride(dataStride), m_Type(type) {
		}

		virtual ~Material() = default;

		const eMaterialType GetType() const { return m_Type; }

		// Get and bind pipeline for material
		const PipelineHandle GetPipeline() const { return m_Pipeline; }
		const uint32_t GetDataStride() const { return m_DataStride; }

	private:
		PipelineHandle m_Pipeline = UINT32_MAX;
		uint32_t m_DataStride = UINT32_MAX;
		eMaterialType m_Type = eMaterialType::kSolidColour;
	};

	// Contains named material objects and stores handles to them to ensure backends don't create duplicates, while allowing app front end to look them up
	class AETHER_API MaterialLibrary
	{
	public:
		MaterialHandle Register(const eMaterialType name, const Material* mat);

		MaterialHandle GetHandle(const eMaterialType name) const;
		const Material& GetMaterial(MaterialHandle handle) const;

		// New funky and apparently thread safe way to do a singleton I found
		static MaterialLibrary& Get();

	private:
		// Cache of our materials and their handles
		std::unordered_map<eMaterialType, MaterialHandle> m_MaterialCache;
		// Container for material objects
		std::vector<const Material*> m_Materials;
	};
}
