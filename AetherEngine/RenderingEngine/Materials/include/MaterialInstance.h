#pragma once
//===============================================================================
// desc: Material instance in Aether - allows us to use a base material to select pipeline + layout and an instance to supply individual values
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "Material.h"
//===============================================================================

namespace Aether
{
	class AETHER_API MaterialInstance
	{
	public:
		MaterialInstance(eMaterialType material);

		void Upload();
		void Update();

		const Material& GetMaterialRef() const;

		eMaterialType GetType() const { return MaterialLibrary::Get().GetMaterial(m_MaterialRef).GetType(); }
		uint32_t GetMaterialIndex() const { return m_MaterialIndex; }
		void SetMaterialIndex(uint32_t index);

		// Accessors for memcpy
		const void* GetRawData() const { return m_Data.data(); }
		void* GetRawData() { return m_Data.data(); }

		// Get the data out of our material instance, templated to account for various materials types at compile-time
		template<typename T>
		T& GetData()
		{
			m_bDirty = true;
			return *reinterpret_cast<T*>(m_Data.data());
			//return std::get<T>(m_Data);
		}

		bool IsDirty() const { return m_bDirty; }
		void ClearDirty() { m_bDirty = false; }

	private:
		MaterialHandle m_MaterialRef;
		std::vector<uint8_t> m_Data;
		//MaterialDataVariant m_Data;
		uint32_t m_MaterialIndex = UINT32_MAX;
		// Dirty flag so we know when to update our material data
		bool m_bDirty = false;
	};
}
