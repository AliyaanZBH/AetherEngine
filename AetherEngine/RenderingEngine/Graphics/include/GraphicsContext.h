#pragma once
//===============================================================================
// desc: A collection of common graphics and rendering primitives
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "AetherUtils.h"
#include "Shader.h"
#include "glm/vec4.hpp" // Include is needed here so that the app can see GLM
//===============================================================================

namespace Aether
{
	// Base Vertex data structs
	struct AETHER_API Vertex
	{
		glm::vec4 m_Pos;
		glm::vec4 m_Colour;
	};

    enum class eVertexAttributeFormat
    {
        kFloat,
        kFloat2,
        kFloat3,
        kFloat4,
        kNumFormats
    };

    struct VertexAttribute
    {
		eShaderSemantic         m_Name;				// Position, Colour, etc.
        eVertexAttributeFormat  m_Format;           // e.g., Float3
        uint32_t                m_Offset;           // Offset in bytes from vertex start
    };

	static uint32_t VertexAttributeSize(eVertexAttributeFormat type)
	{
		switch (type)
		{
			case eVertexAttributeFormat::kFloat:    return 4;
			case eVertexAttributeFormat::kFloat2:   return 4 * 2;
			case eVertexAttributeFormat::kFloat3:   return 4 * 3;
			case eVertexAttributeFormat::kFloat4:   return 4 * 4;
		}

		AETHER_ASSERT(AETHER_FAIL, "Unknown Vertex Attribute Format!");
		return 0;
	}

	static uint32_t VertexAttributeComponentCount(eVertexAttributeFormat type)
	{
		switch (type)
		{
			case eVertexAttributeFormat::kFloat:    return 1;
			case eVertexAttributeFormat::kFloat2:   return 2;
			case eVertexAttributeFormat::kFloat3:   return 3;
			case eVertexAttributeFormat::kFloat4:   return 4;
		}

		AETHER_ASSERT(AETHER_FAIL, "Unknown Vertex Attribute Format!");
		return 0;
	}

    struct VertexLayout
    {
        std::vector<VertexAttribute> m_Attributes;
        uint32_t m_Stride;
    };
}
