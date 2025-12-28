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

		// Check if we want to read back from this in CPU - this defines which type of upload we'll be taking. Default heap gives most bandwidth but no access, Upload is good for single writes and reads.
		defaultHeapProps.Type = desc.m_CPUVisible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
		// Depending on the heap we're using, the initial state will need to be different, with upload heaps needing to start in `GENERIC_READ`, and default ones as a copy_Dest
		D3D12_RESOURCE_STATES initialState = desc.m_CPUVisible ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST;

		CD3DX12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(desc.m_SizeInBytes);

		AETHER_HR_ASSERT(device->CreateCommittedResource(
			&defaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&defaultResourceDesc,
			initialState,
			nullptr,
			IID_PPV_ARGS(&m_Resource)
		));

		// If using default heap (good for large or mostly static buffers), also create an upload heap
		if (defaultHeapProps.Type == D3D12_HEAP_TYPE_DEFAULT)
		{
			CD3DX12_HEAP_PROPERTIES uploadHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			AETHER_HR_ASSERT(m_Device->CreateCommittedResource(
				&uploadHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&defaultResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,	// GPU will read from this buffer and copy its contents to the resource we created above
				nullptr,
				IID_PPV_ARGS(&m_UploadHeap)
			));
		}
	}

	BufferDX12::~BufferDX12()
	{
		if (m_Resource)
			m_Resource->Release();
	}

	D3D12_RESOURCE_STATES BufferDX12::GetFinalState(eBufferType type) const
	{
		switch (type)
		{
		case eBufferType::kVertex:
		case eBufferType::kConstant:
			return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		case eBufferType::kIndex:
			return D3D12_RESOURCE_STATE_INDEX_BUFFER;

		default:
			return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	void BufferDX12::Upload(const void* data, size_t size, size_t offset)
	{
		// Make sure to update our desc first.
		m_Desc.m_Data = data;
		m_Desc.m_SizeInBytes = size;

		if (m_UploadHeap)
		{
			// Default heap: copy to GPU from intermediate upload heap via UpdateSubresources
			D3D12_SUBRESOURCE_DATA subData{};
			subData.pData = m_Desc.m_Data;
			subData.RowPitch = m_Desc.m_SizeInBytes;
			subData.SlicePitch = m_Desc.m_SizeInBytes;
		
			UpdateSubresources(m_CmdList, m_Resource, m_UploadHeap, 0, 0, 1, &subData);
			
			// Transition the resource now that it has been uploaded
			D3D12_RESOURCE_STATES finalState = GetFinalState(m_Desc.m_Type);

			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_Resource, D3D12_RESOURCE_STATE_COMMON, finalState);
			m_CmdList->ResourceBarrier(1, &barrier);
		}
		else
		{
			// Skip upload heap and GPU copy: map & memcpy directly from CPU
			void* mapped = nullptr;
			m_Resource->Map(0, nullptr, &mapped);
			memcpy(static_cast<uint8_t*>(mapped) + offset, m_Desc.m_Data, m_Desc.m_SizeInBytes);
			m_Resource->Unmap(0, nullptr);
		}
	}
	void BufferDX12::SetName(const WCHAR* name)
	{
		m_Resource->SetName(name);
	}
}