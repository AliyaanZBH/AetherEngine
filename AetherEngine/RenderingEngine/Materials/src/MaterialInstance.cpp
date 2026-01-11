#pragma once
//===============================================================================
// desc: Material instance in Aether - allows us to use a base material to select pipeline + layout and an instance to supply individual values
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "MaterialInstance.h"
#include "Renderer.h"
//===============================================================================

namespace Aether
{
	MaterialInstance::MaterialInstance(eMaterialType material)
	{
		m_MaterialRef = MaterialLibrary::Get().GetHandle(material);
		// Guarantee that our instance has enough storage for the material data
		m_Data.resize(MaterialLibrary::Get().GetMaterial(m_MaterialRef).GetDataStride());
	}

	void MaterialInstance::Upload()
	{
		Renderer::UploadMaterialInstance(*this);
	}

	void MaterialInstance::Update()
	{
		Aether::Renderer::UpdateMaterialInstance(*this);
	}

	const Material& MaterialInstance::GetMaterialRef() const
	{
		return MaterialLibrary::Get().GetMaterial(m_MaterialRef);
	}

	void MaterialInstance::SetMaterialIndex(uint32_t index)
	{
		m_MaterialIndex = index;
	}
}
