//===============================================================================
// desc: Implmentation of GPU buffers for DirectX 1
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "BufferDX11.h"
#include "D3DUtils.h"
//===============================================================================

namespace Aether
{
	// Helper functions to bind DX11 buffers
	//

	D3D11_BIND_FLAG BufferDX11::ToDX11BindFlag(eBufferType usage)
	{
		switch (usage)
		{
			case eBufferType::kVertex:				return D3D11_BIND_VERTEX_BUFFER;
			case eBufferType::kIndex:				return D3D11_BIND_INDEX_BUFFER;
			case eBufferType::kConstantPerFrame:
			case eBufferType::kConstantPerDraw:		return D3D11_BIND_CONSTANT_BUFFER;
			case eBufferType::kStructuredStorage:	return D3D11_BIND_SHADER_RESOURCE;

			default:
				AETHER_ASSERT(false, "Unsupported usage for DX11");
				return D3D11_BIND_FLAG{};
		}
	}

	D3D11_RESOURCE_MISC_FLAG BufferDX11::ToDX11MiscFlag(eBufferType usage)
	{
		switch (usage)
		{

			case eBufferType::kStructuredStorage:			return D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		default:
			AETHER_ASSERT(false, "Unsupported usage for DX11");
			return D3D11_RESOURCE_MISC_FLAG{};
		}
	}

	BufferDX11::BufferDX11(const BufferDesc& desc, ID3D11Device* device, ID3D11DeviceContext* context)
		: Buffer(desc), m_Device(device), m_Context(context)
	{
		
		// Define DX11 buffer object using our description
		D3D11_BUFFER_DESC bd{};
		bd.ByteWidth = AETHER_ALIGN16(desc.m_SizeInBytes);	// Ensure 16 byte alignment
		bd.BindFlags = ToDX11BindFlag(desc.m_Type);
		bd.Usage = desc.m_CPUVisible ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = desc.m_CPUVisible ? D3D11_CPU_ACCESS_WRITE : 0;
		bd.MiscFlags = ToDX11MiscFlag(desc.m_Type);
		bd.StructureByteStride = desc.m_StructStride;

		D3D11_SUBRESOURCE_DATA init{ desc.m_Data };

		AETHER_HR_ASSERT(device->CreateBuffer(
			&bd,
			desc.m_Data ? &init : nullptr,
			&m_Buffer
		));

		// Extra step for structured buffers

		if (desc.m_Type == eBufferType::kStructuredStorage)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN; // REQUIRED for structured buffers
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = desc.m_SizeInBytes / desc.m_StructStride;

			device->CreateShaderResourceView(m_Buffer, &srvDesc, &m_SRV);
		}
	}

	BufferDX11::~BufferDX11()
	{
		if (m_Buffer)
			m_Buffer->Release();

		if (m_SRV)
			m_Buffer->Release();
	}

	void BufferDX11::Upload(const void* data, size_t size, size_t offset, bool overwrite)
	{
		// Check if our buffer is able to be written to by the CPU
		if (m_Desc.m_CPUVisible)
		{
			// DX11 uses the device context to map the buffer
			D3D11_MAPPED_SUBRESOURCE mapped = {};
			AETHER_HR_ASSERT(m_Context->Map(
				m_Buffer,
				0,
				overwrite
					? D3D11_MAP_WRITE_DISCARD			// This flag replaces the entire buffer contents.
					: D3D11_MAP_WRITE_NO_OVERWRITE,		// This flag replaces only a single element or region of the buffer
				0,
				&mapped
			));
			
			// Copy into the mapped CPU pointer, just like DX12
			memcpy(static_cast<uint8_t*>(mapped.pData) + offset, data, size);
			
			// Unmap and commit the write
			m_Context->Unmap(m_Buffer, 0);
		}
		else
		{
			// If we have a default buffer, let the GPU handle it
			m_Context->UpdateSubresource(m_Buffer, 0, nullptr, data, 0, 0);
		}
	}
}