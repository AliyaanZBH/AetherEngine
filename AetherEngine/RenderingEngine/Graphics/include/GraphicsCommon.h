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

	struct alignas(16) PerDrawData
	{
		glm::mat4 m_ModelMatrix = glm::mat4(1.f);	// 64
		uint32_t m_MaterialIndex = 0u;				// 4
		uint32_t  _pad0;
		uint32_t  _pad1;
		uint32_t  _pad2;
	};

	namespace ShaderBindings
	{
		constexpr uint32_t kPerDrawCB = 0;		// b0
		constexpr uint32_t kMaterialSRV = 0;	// t0
	}
};
