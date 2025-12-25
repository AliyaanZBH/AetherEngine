#pragma once
//===============================================================================
// desc: Set of common headers that we don't need to build every time!
// auth: Aliyaan Zulfiqar
// ============================================================================

#include <stdio.h>
#include <stdexcept>
#include <cassert>
#include <cstdarg>

#include <functional>
#include <memory>
#include <vector>

#include <string>
#include <sstream>
#include <fstream>

//
//	Our headers that won't change much
//

#include "InputCodes.h"

//
//	Third party Libraries
//

#include <glm/glm.hpp>

//
//	Windows and Renderer specific Third-Party libraries
//

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Need to include glad before GLFW ... OR ELSE!
#ifdef USE_OPENGL
#include <glad/glad.h>
#include <KHR/khrplatform.h>
#endif

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>


#ifdef AETHER_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <wrl/client.h>

	#include <DirectXMath.h>
	#include <d3dcompiler.h>
#ifdef USE_DX11
	#include <d3d11.h>
	#include <dxgi1_3.h>
	#include <imgui_impl_dx11.h>
#endif
#ifdef USE_DX12
	#include <d3d12.h>
	#include <dxgi1_4.h>
	#include <d3dx12.h>
	#include <imgui_impl_dx12.h>
#endif
#endif

