#pragma once
//===============================================================================
// desc: The core rendering app that facilitates DLL exporting for use in the main AetherApp
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "AetherUtils.h"
//===============================================================================

namespace Aether
{
	class AETHER_API Application
	{
	public:
		Application() {}
		virtual ~Application() {}
	
		AetherError Run();
	};

	// External function to be defined in clinet applications (games)
	Application* CreateApplication();
}