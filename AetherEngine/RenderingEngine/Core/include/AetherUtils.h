#pragma once
//===============================================================================
// desc: Contains helper functions and macros for error handling
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"

#include <stdexcept>
#include <sstream>
//===============================================================================

// Debug stuff - currently tied to windows, will update to be truly cross platform in future :)

#if defined(DEBUG) | defined(_DEBUG)

#ifndef AETHER_ASSERT													

inline void AetherError(AETHER_RESULT ar, const char* pFileStr, int lineNum)
{
	const char* output = "";
	switch (ar)
	{
		case AETHER_FAIL:
		{
			output = "AETHER_FAIL - something didn't complete properly.";
			break;
		}
		default:
		{;
			output = "Something broke so hard it didn't even FAIL! I don't know what you've done but it ain't good!!";
			break;
		}
	}

	static char sErrBuffer[256];
	sprintf_s(sErrBuffer, 256, "FILE:%s \rLINE:%i \rERROR:%s", pFileStr, lineNum, output);
	throw std::runtime_error(output);

}

#define AETHER_ASSERT(ret)							\
{													\
		AETHER_RESULT ar = (ret);					\
		if (ar != AETHER_OK)						\
			AetherError(ar, __FILE__, __LINE__);	\
}													\

#endif
#else
#ifndef AETHER_ASSERT
#define AETHER_ASSERT(x) (x)
#endif
#endif