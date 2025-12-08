#pragma once
//===============================================================================
// desc: A collection of key constants, typedefs and macros for debugging, DLL exporting, and other utilities
// auth: Aliyaan Zulfiqar
//===============================================================================

typedef int AETHER_RESULT;	// For more useful error handling at low-level, can be used interchangeably with Windows HResult

#define AETHER_OK 0
#define AETHER_FAIL -1

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