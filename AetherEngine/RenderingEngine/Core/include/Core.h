#pragma once
//===============================================================================
// desc: A collection of macros for DLL exporting
// auth: Aliyaan Zulfiqar
//===============================================================================

#ifdef AETHER_PLATFORM_WINDOWS
	#ifdef AETHER_BUILD_DLL
		#define AETHER_API __declspec(dllexport)
	#else
		#define AETHER_API __declspec(dllimport)
	#endif
#else
	#error Aether currently only supports Windows! Linux and Mac are coming soon <3
#endif

// Bit field macro
#define BIT(x) (1 << x)