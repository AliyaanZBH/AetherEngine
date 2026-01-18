#pragma once
//===============================================================================
// desc: A small helper struct to organise render submission and drawing from the high-level Renderer API to the low-level backends
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Drawing.h"
//===============================================================================

namespace Aether
{
	struct VertexBufferView;
	struct IndexBufferView;
	class Material;

	// Instead of submitting draw functions immediately, submit the necessary data so that they can all be drawn safely at the correct time for each rendering API.
	// This abstraction helps with app users not having to worry about all this when they call a draw function
	struct DrawCommand
	{
		eDrawGeoType m_Type;
		VertexBufferView* m_VBV;
		IndexBufferView* m_IBV;
		MaterialInstance* m_MaterialInstance;
		glm::mat4 m_ModelMatrix;
		glm::mat4 m_NormalMatrix;
		//glm::vec4[4] m_PerVertexColour;
		//const Mesh* m_Mesh = nullptr;
	};
}
