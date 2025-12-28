#pragma once
//===============================================================================
// desc: Shader class implementation for DirectX 11
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ShaderDX11.h"
#include "D3DUtils.h"
//===============================================================================

namespace Aether
{
	ShaderDX11::ShaderDX11(const std::wstring& shaderSrc, const char* shaderType)
	{

		ID3DBlob* errorBuff;    // A buffer holding the error data if any

		// [AZB]: Define the list of directories to search for include files
		std::vector<std::wstring> includeDirs =
		{
			L"Shaders\\DirectX"  // [AZB]: Main HLSL shader directory
		};

		// [AZB]: Create an instance of the custom include handler with the list of directories and the current shader
		CustomIncludeHandler includeHandler(includeDirs, shaderSrc);

		std::wstring workingShaderDir = includeHandler.GetDirectories().back();
		std::wstring fullPathToShader = workingShaderDir + L"\\DirectX\\" + shaderSrc;

		AETHER_HR_ASSERT(D3DCompileFromFile(fullPathToShader.c_str(),
			nullptr,
			&includeHandler,
			"main",
			shaderType,
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&m_ShaderBinary,
			&errorBuff
		));

		// Print errors!
		if (errorBuff != nullptr)
			OutputDebugStringA((char*)errorBuff->GetBufferPointer());
	}
}
