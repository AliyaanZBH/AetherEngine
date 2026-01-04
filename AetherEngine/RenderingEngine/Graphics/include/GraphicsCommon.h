#pragma once
//===============================================================================
// desc: A collection of common graphics and rendering structs
// auth: Aliyaan Zulfiqar
//===============================================================================

namespace Aether
{
	struct alignas(16) PerDrawData
	{
		glm::mat4 m_ModelMatrix = glm::mat4(1.f);	// 64
		glm::vec4 m_Colour = glm::vec4(1.f);		// 16
	};

	struct alignas(16) PerFrameData
	{
		glm::mat4 m_ViewProj = glm::mat4(1.f);		// 64
	};
};
