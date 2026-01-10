#pragma once
//===============================================================================
// desc: Base class for a material in Aether
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	using ShaderHandle = uint32_t;

	enum class eMaterialType {kSolidColour, kTextured, kPBR};

	class AETHER_API Material
	{
	public:
		virtual ~Material() = default;

		virtual void WritePerDrawData(void* dst) = 0;

		// Get and bind the appropriate shader from the library
		virtual const ShaderHandle GetShader() const = 0;
		virtual const eMaterialType GetType() const = 0;
	};
}