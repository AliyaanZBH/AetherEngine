#pragma once
//===============================================================================
// desc: Shader class implementation for DirectX 12
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ShaderDX12.h"
#include "D3DUtils.h"
//===============================================================================

namespace Aether
{
	ShaderDX12::ShaderDX12(const std::wstring& shaderSrc, const std::string& shaderType)
	{

		ID3DBlob* shaderBlob;	// D3D blob for holding shader bytecode
		ID3DBlob* errorBuff;    // A buffer holding the error data if any
		// [AZB]: Define the list of directories to search for include files
		std::vector<std::wstring> includeDirs =
		{
			L"..\\Shaders"  // [AZB]: Main common shader directory, this is where my stuff lives
		};

		// [AZB]: Create an instance of the custom include handler with the list of directories and the current shader
		CustomIncludeHandler includeHandler(includeDirs, shaderSrc);

		std::wstring workingShaderDir = includeHandler.GetDirectories().back();
		std::wstring fullPathToShader = workingShaderDir + L"\\" + shaderSrc;

		D3DCompileFromFile(fullPathToShader.c_str(),
			nullptr,
			&includeHandler,
			"main",
			shaderType.c_str(),
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&shaderBlob,
			&errorBuff
		);

		// Print errors!
		if (errorBuff != nullptr)
			OutputDebugStringA((char*)errorBuff->GetBufferPointer());

		// Fill out shader bytecode struct, which is basically just a pointer to the shader bytecode and the size of the shader bytecode    
		m_ShaderBinary.BytecodeLength = shaderBlob->GetBufferSize();
		m_ShaderBinary.pShaderBytecode = shaderBlob->GetBufferPointer();
	}
}
