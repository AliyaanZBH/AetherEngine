#pragma once
//===============================================================================
// desc: A small helper struct to organise render submission and drawing from the high-level Renderer API to the low-level backends
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "glm/gtc/matrix_transform.hpp"
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include "glm/gtx/euler_angles.hpp"

#include "Material.h"				// For the application to see them
#include "MaterialInstance.h"				// For the application to see them
//===============================================================================

namespace Aether
{
	namespace Colours
	{
		const glm::vec4 kPureBlack = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		const glm::vec4 kPureWhite = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		const glm::vec4 kGunmetalGrey = glm::vec4(0.068f, 0.068f, 0.068f, 0.965f);
		const glm::vec4 kAetherGold = glm::vec4(1.f, 0.8f, 0.f, 1.f);

		const glm::vec4 kPureRed = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		const glm::vec4 kPureGreen = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		const glm::vec4 kPureBlue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		const glm::vec4 kDarkRed = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
		const glm::vec4 kDarkGreen = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
		const glm::vec4 kDarkBlue = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);

		const glm::vec4 kYellow = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		const glm::vec4 kMagenta = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		const glm::vec4 kCyan = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
	};

	struct Transform
	{
		glm::vec3 m_Position = glm::vec3(0.f);
		glm::vec3 m_Rotation = glm::vec3(0.f);		// Rotation currently unsupported
		glm::vec3 m_Scale = glm::vec3(0.f);

		glm::mat4 CreateModelMatrix() const
		{
			glm::mat4 modelMatrix = glm::mat4(1.f);

			glm::mat4 t = glm::translate(glm::mat4(1.f), m_Position);
			glm::mat4 r = glm::eulerAngleZYX(m_Rotation.x, m_Rotation.y, m_Rotation.z);
			glm::mat4 s = glm::scale(glm::mat4(1.f), m_Scale);

			modelMatrix = t  * r  * s;
			return modelMatrix;
		}

		glm::mat4 CreateNormalMatrix() const
		{
			// This will implicitly cast the model matrix to a 3x3 which is all we need
			glm::mat4 modelMatrix = CreateModelMatrix();
			return glm::transpose(glm::inverse(modelMatrix));
		}
	};


	struct alignas(16) FlatColourMaterialData
	{
		glm::vec4 m_Colour = glm::vec4(1.f);		// 16
	};

	// Uses a modern Blinn-Phong implementatin
	struct alignas(16) LitColourMaterialData
	{
		glm::vec4 m_Diffuse = Colours::kAetherGold;		// 16
		glm::vec3 m_SpecularR0 = glm::vec3(0.05f);		// 12
		float m_Roughness = 0.5f;						// 4
	};

	struct alignas(16) PerDrawData_Textured
	{
		glm::vec4 m_Tint = glm::vec4(1.f);			// 16
		uint32_t m_TextureID = 0u;					// 4
	};

	struct alignas(16) PerDrawData_PBR
	{
		float m_Roughness;			// 4
		float m_Metallic;			// 4
		uint32_t m_DiffuseTexID;	// 4
		uint32_t m_NormalTexID;		// 4
	};


	enum class eDrawGeoType { kLine, kTri, kQuad, kCircle, kCube, kMesh };

}