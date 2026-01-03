#pragma once
//===============================================================================
// desc: Shader class implementation for DirectX 11
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Shader.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ShaderDX11 final : public Shader
	{
	public:
		ShaderDX11(const std::wstring& shaderSrc, const char* shaderType);

		ID3DBlob* Get() { return m_ShaderBinary; }
	private:

		// Actual binary blob that is our compiled shader
		ID3DBlob* m_ShaderBinary = nullptr;
	};
}
