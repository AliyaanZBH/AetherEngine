//===============================================================================
// desc: DX11 pipeline object definition
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "PipelineDX11.h"
//===============================================================================

namespace Aether
{
	PipelineDX11::PipelineDX11(ShaderDX11* vs, ShaderDX11* ps, std::vector<D3D11_INPUT_ELEMENT_DESC> inputs, ID3D11Device* device)
	{
		// Actually create specific shader from the binary blobs
		device->CreateVertexShader(vs->Get()->GetBufferPointer(), vs->Get()->GetBufferSize(), nullptr, &m_VS);
		device->CreatePixelShader(ps->Get()->GetBufferPointer(), ps->Get()->GetBufferSize(), nullptr, &m_PS);

		// Create input layout
		device->CreateInputLayout(inputs.data(), UINT(inputs.size()), vs->Get()->GetBufferPointer(), vs->Get()->GetBufferSize(), &m_Layout);
	}
}
