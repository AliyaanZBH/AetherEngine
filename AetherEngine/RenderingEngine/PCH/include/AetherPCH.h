#pragma once
//===============================================================================
// desc: Set of common headers that we don't need to build every time!
// auth: Aliyaan Zulfiqar
// ============================================================================

#include <stdio.h>
#include <stdexcept>
#include <cassert>

#include <functional>
#include <memory>
#include <vector>

#include <string>
#include <sstream>
#include <fstream>

#ifdef AETHER_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <wrl/client.h>

	#include <dxgi1_3.h>
	#include <d3dcompiler.h>
#ifdef USE_DX11
	#include <d3d11.h>
#endif
#ifdef USE_DX12
	#include <d3d12.h>
#endif
#endif
