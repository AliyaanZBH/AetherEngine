//===============================================================================
// desc: Simple solid colour material
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "MaterialSolidColour.h"
#include "Shader.h"
#include "GraphicsCommon.h"
//===============================================================================

namespace Aether
{
	MaterialSolidColour::MaterialSolidColour(const glm::vec4& colour)
		: m_Colour(colour) {}

	void MaterialSolidColour::WritePerDrawData(void* dst)
	{
		auto* data = static_cast<PerDrawData_Solid*>(dst);
		data->m_Colour = m_Colour;
	}

	const ShaderHandle MaterialSolidColour::GetShader() const
	{
		return ShaderLibrary::Get().GetHandle("SolidColour");
	}
}
