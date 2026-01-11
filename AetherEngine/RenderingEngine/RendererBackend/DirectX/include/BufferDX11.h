#pragma once
//===============================================================================
// desc: Implmentation of GPU buffers for DirectX 11
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Buffer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API BufferDX11 final : public Buffer
	{
	public:
		BufferDX11(const BufferDesc& desc, ID3D11Device* device, ID3D11DeviceContext* context);
		~BufferDX11();

		void Upload(const void* data, size_t size, size_t offset = 0) override;

		ID3D11Buffer* GetBuffer() const { return m_Buffer; }
		ID3D11ShaderResourceView* GetSRV() const { return m_SRV; }
	private:

		// Helper function to bind DX11 buffers
		D3D11_BIND_FLAG ToDX11BindFlag(eBufferType usage);
		D3D11_RESOURCE_MISC_FLAG ToDX11MiscFlag(eBufferType usage);

		ID3D11Device* m_Device = nullptr;
		ID3D11DeviceContext* m_Context = nullptr;
		ID3D11Buffer* m_Buffer = nullptr;
		ID3D11ShaderResourceView* m_SRV = nullptr;
	};
}