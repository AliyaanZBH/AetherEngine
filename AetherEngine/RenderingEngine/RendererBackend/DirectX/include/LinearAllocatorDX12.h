#pragma once
//===============================================================================
// desc: Implmentation of a simple linear allocator for constant buffers in DirectX 12
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "D3DUtils.h"
//===============================================================================
namespace Aether
{
    struct LinearAllocatorDX12
    {
        ID3D12Resource* m_Resource = nullptr;   // Persistent upload heap
        uint8_t*        m_MappedPtr = nullptr;  // Mapped pointer
        size_t          m_AlignedSize = 0;      // Per-slice CB size
        size_t          m_Offset = 0;           // Current linear offset
        uint8_t         m_CurrentIndex = 0;           // Current descriptor slot index
        size_t          m_Capacity = 0;         // Total heap size

        void Init(ID3D12Device* device, size_t sliceSize, size_t maxSlices)
        {
            // Constant buffers need 256 byte alignment
            m_AlignedSize = AETHER_ALIGN256(sliceSize);
            m_Capacity = m_AlignedSize * maxSlices;

            CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(m_Capacity);
            AETHER_HR_ASSERT(device->CreateCommittedResource(
                &heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                IID_PPV_ARGS(&m_Resource)
            ));

            // map once
            m_Resource->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedPtr));
            m_Offset = 0;
        }

        D3D12_GPU_VIRTUAL_ADDRESS Alloc(const void* data)
        {
            if (m_Offset + m_AlignedSize > m_Capacity)
                throw std::runtime_error("CB linear allocator overflow");

            memcpy(m_MappedPtr + m_Offset, data, m_AlignedSize);
            D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = m_Resource->GetGPUVirtualAddress() + m_Offset;
            m_Offset += m_AlignedSize;
            return gpuAddr;
        }

        void Reset() { m_Offset = 0; m_CurrentIndex = 0; }

        const uint8_t GetIndex() const { return m_CurrentIndex; }
        void IncrementIndex() { m_CurrentIndex++; }
    };
};
