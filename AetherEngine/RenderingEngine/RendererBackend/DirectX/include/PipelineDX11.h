#pragma once
//===============================================================================
// desc: DX11 pipeline object definition
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Pipeline.h"
#include "ShaderDX11.h"
//===============================================================================

namespace Aether
{
	class AETHER_API PipelineDX11 final : public Pipeline
	{
	public:

		explicit PipelineDX11(ShaderDX11* vs, ShaderDX11* ps, std::vector<D3D11_INPUT_ELEMENT_DESC> inputs, ID3D11Device* device);

		ID3D11VertexShader* GetVS() const { return m_VS; }
		ID3D11PixelShader*  GetPS() const {return m_PS; }
		ID3D11InputLayout*  GetInputLayout() const {return m_Layout; }
	private:
		ID3D11VertexShader* m_VS = nullptr;
		ID3D11PixelShader*  m_PS = nullptr;
		ID3D11InputLayout*  m_Layout = nullptr;
	};

}
