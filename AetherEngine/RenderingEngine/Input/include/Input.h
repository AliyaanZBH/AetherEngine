#pragma once
//===============================================================================
// desc: The core input polling class that handles all inputs, based largely on GLFW for now.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "InputCodes.h"
#include "glm/vec2.hpp"
//===============================================================================

namespace Aether
{
	class AETHER_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		//static bool IsKeyReleased(KeyCode key);
	
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}