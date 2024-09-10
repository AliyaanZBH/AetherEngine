#pragma once
//===============================================================================
// desc: The core rendering app that facilitates DLL exporting for use in the main AetherApp
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	class AETHER_API Application
	{
	public:
		Application() {}
		virtual ~Application() {}
	
		void Run();
	};
	
	
}