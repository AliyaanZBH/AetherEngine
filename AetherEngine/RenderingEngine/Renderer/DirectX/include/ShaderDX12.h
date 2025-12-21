#pragma once
//===============================================================================
// desc: Shader class implementation for DirectX 12
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Shader.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ShaderDX12 final : public Shader
	{
	public:
		ShaderDX12(const std::wstring& shaderSrc, const std::string& shaderType);

		void Bind() override {}
		void Unbind() override {}

		D3D12_SHADER_BYTECODE& Get() { return m_ShaderBinary; }
	private:

		// Actual binary blob that is our compiled shader
		D3D12_SHADER_BYTECODE m_ShaderBinary;
	};
}