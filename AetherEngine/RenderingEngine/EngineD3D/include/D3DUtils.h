#pragma once
//===============================================================================
// desc: A collection of handy debug utilies for DirectX/WinAPI, chiefly the ability to get better debugging info from HResults!
// auth: Aliyaan Zulfiqar
//===============================================================================
#if USE_DX11
#include <d3d11.h>
#endif

#if USE_DX12
#include <d3d12.h>
#endif

#include <stdio.h>
//===============================================================================

// Credit: Mark Featherstone
#if defined(DEBUG) | defined(_DEBUG)
#ifndef AETHER_HR_ASSERT													
inline void DXError(HRESULT hr, const char* pFileStr, int lineNum)
{
	LPSTR output;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&output, 0, NULL);
	static char sErrBuffer[256];
	sprintf_s(sErrBuffer, 256, "FILE:%s \rLINE:%i \rERROR:%s", pFileStr, lineNum, output);
	MessageBox(0, sErrBuffer, "Error", 0);
	assert(false);
}

/*
Makes checking if functions worked neater
	e.x.
		HR(SomeD3DFunction());
If it fails the error checking code will get the error message, the
line number and file name and display it all. Saves us typing the
same thing over and over. The app will be stopped.
Just saves typing and stops silly bugs.
*/

// Credit: Mark Featherstone
#define AETHER_HR_ASSERT(x)												\
{															\
	HRESULT hr = (x);										\
	if(FAILED(hr))											\
		DXError( hr, __FILE__, __LINE__);					\
}

#endif
#else
#ifndef AETHER_HR_ASSERT
#define AETHER_HR_ASSERT(x) (x)
#endif
#endif 