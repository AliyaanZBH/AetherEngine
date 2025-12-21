//===============================================================================
// desc: Implmentation of GPU buffers for DirectX 12
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "BufferDX12.h"
#include "D3DUtils.h"
//===============================================================================

namespace Aether
{
	BufferDX12::BufferDX12(const BufferDesc& desc, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
		: Buffer(desc), m_Device(device), m_CmdList(cmdList)
	{
		CD3DX12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		defaultHeapProps.Type = desc.m_CPUVisible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

		CD3DX12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(desc.m_SizeInBytes);

		AETHER_HR_ASSERT(device->CreateCommittedResource(
			&defaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&defaultResourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_Resource)
		));

		// If using default heap (good for large or mostly static buffers), also create an upload heap
		if (defaultHeapProps.Type == D3D12_HEAP_TYPE_DEFAULT)
		{
			CD3DX12_HEAP_PROPERTIES uploadHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			m_Device->CreateCommittedResource(
				&uploadHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&defaultResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,	// GPU will read from this buffer and copy its contents to the resource we created above
				nullptr,
				IID_PPV_ARGS(&m_UploadHeap)
			);
		}
	}

	BufferDX12::~BufferDX12()
	{
		if (m_Resource)
			m_Resource->Release();
	}

	void BufferDX12::Upload(const void* data, size_t size, size_t offset)
	{
		if (m_UploadHeap)
		{
			// Default heap: copy to GPU from intermediate upload heap via UpdateSubresources
			D3D12_SUBRESOURCE_DATA subData{};
			subData.pData = data;
			subData.RowPitch = size;
			subData.SlicePitch = size;

			UpdateSubresources(m_CmdList, m_Resource, m_UploadHeap, 0, 0, 1, &subData);
		}
		else
		{
			// Already in upload heap: map & memcpy
			void* mapped = nullptr;
			m_Resource->Map(0, nullptr, &mapped);
			memcpy(static_cast<uint8_t*>(mapped) + offset, data, size);
			m_Resource->Unmap(0, nullptr);
		}
	}
	void BufferDX12::SetName(const WCHAR* name)
	{
		m_Resource->SetName(name);
	}
}