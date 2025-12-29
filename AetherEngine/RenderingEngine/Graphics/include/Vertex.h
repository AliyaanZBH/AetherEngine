#pragma once
//===============================================================================
// desc: A collection of common vertex data and helpers for buffers and shaders
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "AetherUtils.h"
#include "Shader.h"
#include "glm/glm.hpp"
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
		uint32_t                m_Offset;           // Offset in bytes from vertex start. Can be skipped as the layout struct will calculate this automatically
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

	// Mainly used for openGL vertex attrib arrays, but might be useful elsewhere so I left it defined here.
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

		VertexLayout(std::vector<VertexAttribute> elements)
			: m_Attributes(elements)
		{
			uint32_t offset = 0;
			// Calculate the offsets of each element
			for (VertexAttribute& elem : m_Attributes)
			{
				elem.m_Offset = offset;
				offset += VertexAttributeSize(elem.m_Format);
			}

			// Set stride - this is a vertex layout so it's the size of vertex!
			m_Stride = sizeof(Vertex);
		}
	};
};