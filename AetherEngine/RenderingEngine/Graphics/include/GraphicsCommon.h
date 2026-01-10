#pragma once
//===============================================================================
// desc: A collection of common graphics and rendering structs
// auth: Aliyaan Zulfiqar
//===============================================================================

namespace Aether
{
	struct alignas(16) PerFrameData
	{
		glm::mat4 m_ViewProj = glm::mat4(1.f);		// 64
	};

	struct alignas(16) PerDrawData_Solid
	{
		glm::mat4 m_ModelMatrix = glm::mat4(1.f);	// 64
		glm::vec4 m_Colour = glm::vec4(1.f);		// 16
	};

	struct alignas(16)  PerDrawData_Textured
	{
		glm::vec4 m_Tint = glm::vec4(1.f);			// 16
		uint32_t m_TextureID = 0u;					// 4
	};

	struct alignas(16)  PerDrawData_PBR
	{
		float m_Roughness;			// 4
		float m_Metallic;			// 4
		uint32_t m_DiffuseTexID;	// 4
		uint32_t m_NormalTexID;		// 4
	};
};
