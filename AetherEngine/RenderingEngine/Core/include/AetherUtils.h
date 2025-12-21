#pragma once
//===============================================================================
// desc: Contains helper functions and macros for error handling
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "Log.h"

#include <cstdarg>
//===============================================================================

// Debug stuff - currently tied to windows, will update to be truly cross platform in future :)

#if defined(AETHER_DEBUG) | defined(AETHER_RELEASE_DBG_INFO) | defined(_DEBUG)

#ifndef AETHER_ASSERT													

// Neat little map we can use to store some strings that map to error messages
static std::unordered_map<AETHER_RESULT, std::string> errorMessages =
{
	{AETHER_OK, "Success"},
	{AETHER_FAIL, "AETHER_FAIL - something didn't complete properly."},
	{AETHER_UNKNOWN, "Unknown error - something broke so hard it didn't even FAIL! I don't know what you've done but it ain't good!!"}
};

inline const char* AetherResultToString(AETHER_RESULT result)
{
	auto it = errorMessages.find(result);
	if (it != errorMessages.end())
	{
		return it->second.c_str();
	}
	return "Unknown error code";
}
inline void AetherError(AETHER_RESULT ar, const char* pFileStr, int lineNum, ...)
{
	const char* errorCode = AetherResultToString(ar);
	char errorMessage[512];
	const char* fmt = "%s";

	// Way to get variadic arguments inside a regular non-macro function
	va_list args;
	va_start(args, fmt);

	// Use vsnprintf to format the message and pull out the args
	vsnprintf(errorMessage, sizeof(errorMessage), fmt, args);

	// Finalise and cleanup the va_list
	va_end(args);

	AETHER_ERROR("Assert Failed with code: {0} \n File: {1}, \n Line: {2}, \n Message: {3}", errorCode, pFileStr, lineNum, errorMessage);
	__debugbreak();
}

#define AETHER_ASSERT(ret, ...)									\
{																\
	AETHER_RESULT ar = AETHER_OK;								\
	ar = (ret);													\
	if (ar != AETHER_OK)										\
	{															\
		AetherError(ar, __FILE__, __LINE__, __VA_ARGS__);		\
	}															\
}																\

#endif
#else
#ifndef AETHER_ASSERT
#define AETHER_ASSERT(x, ...) (x)
#endif
#endif