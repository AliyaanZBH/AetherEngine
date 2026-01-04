#pragma once
//===============================================================================
// desc: A small helper struct to organise render submission and drawing from the high-level Renderer API to the low-level backends
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "glm/gtc/matrix_transform.hpp"
//===============================================================================

namespace Aether
{
	struct Transform
	{
		glm::vec3 m_Position = glm::vec3(0.f);
		//glm::vec3 m_Rotation = glm::vec3(1.f);		// Rotation currently unsupported
		glm::vec3 m_Scale = glm::vec3(0.f);

		glm::mat4 CreateModelMatrix() const
		{
			glm::mat4 modelMatrix = glm::mat4(1.f);

			glm::mat4 t = glm::translate(glm::mat4(1.f), m_Position);
			//glm::mat4 r = glm::rotate(glm::mat4(1.f), m_Rotation);
			glm::mat4 s = glm::scale(glm::mat4(1.f), m_Scale);

			modelMatrix = t /* * r */ * s;
			return modelMatrix;
		}
	};

	namespace Colours
	{
		const glm::vec4 kPureBlack = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		const glm::vec4 kPureWhite = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

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
}