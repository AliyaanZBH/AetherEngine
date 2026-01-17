//===============================================================================
// desc: Base class for a material in Aether, which defines a shading pipeline. There should only be a few of these, with many instances of a given material populating the scene.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Material.h"
#include "AetherUtils.h"
#include "Drawing.h"
//===============================================================================
namespace Aether
{
	void Material::InitialiseDefaults(void* instanceData) const
	{
		switch (m_Type)
		{
			case eMaterialType::kSolidColour:
			{
				*reinterpret_cast<FlatColourMaterialData*>(instanceData) = FlatColourMaterialData();
				break;
			}

			case eMaterialType::kBlinnPhong:
			{
				*reinterpret_cast<LitColourMaterialData*>(instanceData) = LitColourMaterialData();
			}
		}
	}

	MaterialHandle MaterialLibrary::Register(const eMaterialType name, const Material* mat)
	{
		// Check if it's already been registered, don't want to do this twice
		auto it = m_MaterialCache.find(name);
		if (it != m_MaterialCache.end())
			return it->second;

		// Register a new pipeline, grab the current size of the library to ensure we don't overwrite an existing handle (these are created incrementally currently)
		MaterialHandle handle = m_Materials.size();

		// Insert into cache and push back the description
		m_MaterialCache[name] = handle;
		m_Materials.push_back(mat);

		return handle;
	}

	MaterialHandle MaterialLibrary::GetHandle(const eMaterialType name) const
	{
		auto it = m_MaterialCache.find(name);
		AETHER_RESULT ar = !(it != m_MaterialCache.end());	// Not here as we use 0 for success, while this maps to false as a bool.
		AETHER_ASSERT(ar, "Material not registered");
		return it->second;
	}

	const Material& MaterialLibrary::GetMaterial(MaterialHandle handle) const
	{
		AETHER_RESULT ar = !(handle < m_Materials.size());
		AETHER_ASSERT(ar, "Invalid material handle");
		return *m_Materials[handle];
	}

	MaterialLibrary& MaterialLibrary::Get()
	{
		static MaterialLibrary instance;
		return instance;
	}
}
