#pragma once
//===============================================================================
// desc: The core input polling class that handles all inputs, API agnostic
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	class AETHER_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
	private:
		static Input* s_Instance;
	};
}