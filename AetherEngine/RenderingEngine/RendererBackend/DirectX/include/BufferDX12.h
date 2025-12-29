#pragma once
//===============================================================================
// desc: Implmentation of GPU buffers for DirectX 12
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Buffer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API BufferDX12 final : public Buffer
	{
	public:
		BufferDX12(const BufferDesc& desc, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
		~BufferDX12();

		void Upload(const void* data, size_t size, size_t offset = 0) override;
		void SetName(const WCHAR* name);


		ID3D12Resource* GetResource() const { return m_Resource; }
	private:
		D3D12_RESOURCE_STATES GetFinalState(eBufferType type) const;

		ID3D12Device* m_Device = nullptr;
		ID3D12GraphicsCommandList* m_CmdList = nullptr;
		ID3D12Resource* m_Resource = nullptr;
		ID3D12Resource* m_UploadHeap = nullptr;
	};
}