#pragma once
//===============================================================================
// desc: Simple solid colour material
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Material.h"
//===============================================================================

namespace Aether
{
	class AETHER_API MaterialSolidColour final : public Material
	{
	public:
		MaterialSolidColour(const glm::vec4& colour);

		void WritePerDrawData(void* dst) override;
		const ShaderHandle GetShader() const override;
		const eMaterialType GetType() const override { return eMaterialType::kSolidColour; }

		void SetColour(const glm::vec4& colour) { m_Colour = colour; }
	private:
		glm::vec4 m_Colour;
	};
};
