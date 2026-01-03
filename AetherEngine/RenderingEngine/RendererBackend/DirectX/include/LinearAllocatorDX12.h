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
        ID3D12Resource* m_Resource = nullptr;   // Buffer that we are allocating for, needs to be an upload heap
        uint8_t*        m_MappedPtr = nullptr;  // Mapped pointer
        size_t          m_AlignedSize = 0;      // Per-slice CB size
        size_t          m_Offset = 0;           // Current linear offset
        uint8_t         m_CurrentIndex = 0;     // Current descriptor slot index
        size_t          m_Capacity = 0;         // Total heap size

        void Init(ID3D12Resource* resource, size_t sliceSize, size_t maxSlices)
        {
            // Constant buffers need 256 byte alignment
            m_AlignedSize = AETHER_ALIGN256(sliceSize);
            m_Capacity = m_AlignedSize * maxSlices;
            m_Resource = resource;

            // Map once and leave it as this data will change every frame
            m_Resource->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedPtr));
            m_Offset = 0;
        }

        D3D12_GPU_VIRTUAL_ADDRESS Alloc(const void* data)
        {
            memcpy(m_MappedPtr + m_Offset, data, m_AlignedSize);
            D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = m_Resource->GetGPUVirtualAddress() + m_Offset;
            m_Offset += m_AlignedSize;
            return gpuAddr;
        }

        void Reset() { m_Offset = 0; m_CurrentIndex = 0; }
        void IncrementIndex() { m_CurrentIndex++; }

        const uint8_t GetIndex() const { return m_CurrentIndex; }
    };
};
