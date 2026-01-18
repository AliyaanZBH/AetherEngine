#pragma once
//===============================================================================
// desc: A collection of common graphics and rendering structs
// auth: Aliyaan Zulfiqar
//===============================================================================

namespace Aether
{
	struct alignas(16) PerFrameData
	{
		glm::mat4 m_ViewProj = glm::mat4(1.f);			// 64
		glm::vec4 m_CameraPos = glm::vec4(1.f);			// 16
		glm::vec4 m_SunlightPos = glm::vec4(1.f);		// 16
	};

	struct alignas(16) PerDrawData
	{
		glm::mat4 m_ModelMatrix = glm::mat4(1.f);	// 64
		glm::mat4 m_NormalMatrix = glm::mat4(1.f);	// 64
		uint32_t m_MaterialIndex = 0u;				// 4
		//float _pad0, _pad1;							// 8
	};

	namespace ShaderBindings
	{
		constexpr uint32_t kPerDrawCB = 0;		// b0
		constexpr uint32_t kMaterialSRV = 0;	// t0
	}
};
