#pragma once
//===============================================================================
// desc: A collection of key constants, typedefs and macros for debugging, DLL exporting, and other utilities
// auth: Aliyaan Zulfiqar
//===============================================================================

typedef int AETHER_RESULT;	// For more useful error handling at low-level, can be used interchangeably with Windows HResult

#define AETHER_OK 0
#define AETHER_FAIL -1
#define AETHER_UNKNOWN -99

#ifdef AETHER_PLATFORM_WINDOWS
	#ifdef AETHER_DYNAMIC_LINK
		#ifdef AETHER_BUILD_DLL
			#define AETHER_API __declspec(dllexport)
		#else	// If not building DLL, we're importing it!
			#define AETHER_API __declspec(dllimport)
		#endif
	#else
		#define AETHER_API
	#endif
#else
	#error Aether currently only supports Windows! Linux and Mac are coming soon <3
#endif

// Bit field macro
#define BIT(x) (1 << x)

// Nice helper for alignment
#define AETHER_ALIGN16(x) (((x) + 15) & ~15)	// Rounds up to the nearest 16-byte boundary
#define AETHER_ALIGN256(x) (((x) + 255) & ~255)	// Rounds up to the nearest 256-byte boundary