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

		// Check if we want to read back from this in CPU - this defines which type of upload path we'll be taking. 
		// Default heap is good for static data that won't change much
		// Upload is good for constantly changing data such as constant buffers
		CD3DX12_HEAP_PROPERTIES heapProps = desc.m_CPUVisible ? CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD) : CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		// Depending on the heap we're using, the initial state will need to be different, with upload heaps needing to start in `GENERIC_READ`, and default ones as a copy_Dest
		//D3D12_RESOURCE_STATES initialState = desc.m_CPUVisible ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST;
		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

		// Constant buffers need 256 byte alignment and enough additional capacity to handle multiple draws per frame
		UINT capacity = 1;
		if (m_Desc.m_Type == eBufferType::kConstantPerFrame || m_Desc.m_Type == eBufferType::kConstantPerDraw)
		{
			m_Desc.m_SizeInBytes = AETHER_ALIGN256(desc.m_SizeInBytes);

			if (m_Desc.m_Type == eBufferType::kConstantPerDraw)
				capacity =  1024;	// 1024 draws per frame
		}

		CD3DX12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_Desc.m_SizeInBytes * capacity);

		AETHER_HR_ASSERT(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&defaultResourceDesc,
			initialState,
			nullptr,
			IID_PPV_ARGS(&m_Resource)
		));

		// If using default heap (good for large or mostly static buffers), also create an intermediate upload heap for use
		if (heapProps.Type == D3D12_HEAP_TYPE_DEFAULT)
		{
			CD3DX12_HEAP_PROPERTIES uploadHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			AETHER_HR_ASSERT(m_Device->CreateCommittedResource(
				&uploadHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&defaultResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,	// GPU will read from this buffer and copy its contents to the resource we created above
				nullptr,
				IID_PPV_ARGS(&m_IntermediateUploadHeap)
			));
		}
		else
		{
			// Map the upload heap once here to be optimal
			m_Resource->Map(0, nullptr, &m_MappedPtr);
		}
	}

	BufferDX12::~BufferDX12()
	{
		if (m_Resource)
		{
			if (m_MappedPtr)
			{
				m_Resource->Unmap(0, nullptr);
				m_MappedPtr = nullptr;
			}

			m_Resource->Release();
		}
	}

	D3D12_RESOURCE_STATES BufferDX12::GetFinalState(eBufferType type) const
	{
		switch (type)
		{
		case eBufferType::kVertex:
		case eBufferType::kConstantPerFrame:
		case eBufferType::kConstantPerDraw:
			return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		case eBufferType::kIndex:
			return D3D12_RESOURCE_STATE_INDEX_BUFFER;

		default:
			return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	void BufferDX12::Upload(const void* data, size_t size, size_t offset, bool overwrite)
	{

		if (m_Desc.m_Type == eBufferType::kConstantPerDraw)
		{
			// DX12 backend will use a special linear allocator to handle constant buffers per draw.
			return;
		}
			
		// If the upload heap exists, this means we are using the DEFAULT heap path. This requires updating subresources via an intermediate upload heap
		if (m_IntermediateUploadHeap)
		{
			// Copy to GPU from intermediate upload heap via UpdateSubresources
			D3D12_SUBRESOURCE_DATA subData{};
			subData.pData = data;
			subData.RowPitch = size;
			subData.SlicePitch = size;

			// This command will put the resource into state COPY_DEST
			UpdateSubresources(m_CmdList, m_Resource, m_IntermediateUploadHeap, 0, 0, 1, &subData);

			// Transition the resource now that it has been uploaded
			D3D12_RESOURCE_STATES finalState = GetFinalState(m_Desc.m_Type);

			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_Resource, D3D12_RESOURCE_STATE_COPY_DEST, finalState);
			m_CmdList->ResourceBarrier(1, &barrier);
		}
		else // If there is no intermediate upload heap, that means our buffer is already an upload heap, and we can map from CPU to GPU directly.
		{
			// Skip intermediate upload heap and GPU copy: map & memcpy directly from CPU
			memcpy(static_cast<uint8_t*>(m_MappedPtr) + offset, data, size);
		}
	}

	void BufferDX12::SetName(const WCHAR* name)
	{
		m_Resource->SetName(name);
	}
}