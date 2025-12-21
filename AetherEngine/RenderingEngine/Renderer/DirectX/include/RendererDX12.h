#pragma once
//===============================================================================
// desc: The DX12 engine, handles everything backend related.
//		 Based upon one built for a short challenge, but the goal is to optimize and modernize things going forward.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IRenderer.h"
#include "ShaderDX12.h"
#include "BufferDX12.h"
//===============================================================================

namespace Aether
{
	class RendererDX12 final : public IRenderer
	{
	public:
		// Main start up function
		AETHER_RESULT Initialize(IWindow& window) override;
		void ClearFrame() override;
		void Render() override;
		void Present() override;

		void Resize(int newWidth, int newHeight) override;
		void Terminate() override;

		Buffer* CreateBuffer(const BufferDesc& desc) override;


		void* GetNativeDevice() override { return m_Device.Get(); }
		void* GetNativeContext() override { return 0; }

		void InitImGui() override;
		void BeginImGuiRender() override;
		void EndImGuiRender() override;

		// Public accessors
		//

		// Get a handle to the GPU
		const Microsoft::WRL::ComPtr<ID3D12Device>& GetDevice() { return m_Device; }

	private:

		//
		// Heavy lifting to start D3D12
		//

		//	A device is used to create resources, this essentially represents our GPU
		AETHER_RESULT CreateDevice();

		AETHER_RESULT CreateRenderTargets();
		
		AETHER_RESULT CreateDepthStencil();

		AETHER_RESULT CreatePipelines();

		AETHER_RESULT CompileShaders();

		AETHER_RESULT CreateInputLayoutAndPSO();

		AETHER_RESULT CreateAndUploadGeo();

		// Extra Methods to help with rendering
		//

		AETHER_RESULT CleanupRenderBuffers();

		AETHER_RESULT UpdateViewportAndScissor();

		// Update command lists and clear the frame
		AETHER_RESULT ClearAndSyncFrame();

		// Ensure the CPU frame is no longer in use by the GPU to free the command list
		AETHER_RESULT BeginFrame();

		// Wait for all submitted work to finish, fully flushing the GPU
		AETHER_RESULT WaitForGPU();


		//
		// Temp Structs
		//

		// Simple free list based allocator - taken from ImGui example
		// TODO: Write a proper one!
		struct ImGuiExampleDescriptorHeapAllocator
		{
			ID3D12DescriptorHeap* Heap = nullptr;
			D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
			D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu = {};
			D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu = {};
			UINT                        HeapHandleIncrement = {};
			ImVector<int>               FreeIndices = {};

			void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
			{
				IM_ASSERT(Heap == nullptr && FreeIndices.empty());
				Heap = heap;
				D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
				HeapType = desc.Type;
				HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
				HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
				HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
				FreeIndices.reserve((int)desc.NumDescriptors);
				for (int n = desc.NumDescriptors; n > 0; n--)
					FreeIndices.push_back(n - 1);
			}
			void Destroy()
			{
				Heap = nullptr;
				FreeIndices.clear();
			}
			void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
			{
				IM_ASSERT(FreeIndices.Size > 0);
				int idx = FreeIndices.back();
				FreeIndices.pop_back();
				out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
				out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
			}
			void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
			{
				int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
				int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
				IM_ASSERT(cpu_idx == gpu_idx);
				FreeIndices.push_back(cpu_idx);
			}
		};

		// Private members to facilitate the above functions
		//

		static const DXGI_FORMAT m_kRTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT m_kDSVFormat = DXGI_FORMAT_D32_FLOAT;
		static const UINT8 m_kSRVHeapSize = 64u;
		static const UINT8 m_kNumFrameBuffers = 2u;
		const float m_kClearColour[3] = { 0.1f, 0.2f, 1.0f };

		// Main handle used to create resources and access D3D
		Microsoft::WRL::ComPtr<IDXGIFactory2> m_DXGIFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain = nullptr;

		DXGI_SAMPLE_DESC m_SampleDesc;																					// Multi sampling info - need to store this so that it can be used in pipeline setup too

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue;															// Container for command lists//
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CmdAllocators[m_kNumFrameBuffers];								// Memory management for command lists, can use MT by having enough allocators per buffer per thread (forget MT for now, just single threaded and enough for double buffer)// Currently unused values that will be implemented soon
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CmdList;													// Actual command list to record draw calls into!//

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;															// Container for render targets// Position, height, width, min+max depth of the view we are rendering
		Microsoft::WRL::ComPtr<ID3D12Resource> m_RenderTargets[m_kNumFrameBuffers];

		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;													// Depth stencil!
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSBHeap;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVHeap;															// SRVs!
		static inline ImGuiExampleDescriptorHeapAllocator m_SRVHeapAllocator;											// SRVs allocator - currently using the stock ImGui example one

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;																	// Sync object, only using one to simplify sync between CPU and GPU
		HANDLE m_FenceEvent;																							// Handle that is used when the fence is locked or unlocked
		UINT64 m_FenceValue[m_kNumFrameBuffers];																		// Incremented every frame, each command allocator needs it's own fence
		UINT64 m_GlobalFenceValue = 0;																					// Global value to keep everything in check
		UINT8 m_FrameContextIndex = 0;																					// Current frame context index - used for syncing and is different to the back buffer index

		UINT64 m_RTVDescripterSize;																						// Size of descriptors for our RTVS - these are the same size.
		UINT64 m_GlobalFrameCount = 0;																					// Frame count - how many we rendered so far?
		UINT8 m_BackBufferIndex = 0;																					// Either 0 or 1, points to the current back buffer we're writing to


		//
		//	Window data!
		//
		Aether::IWindow::WinData m_WinData = {};

		//
		//	Drawing!
		//


		// PSO and root sig!
		ID3D12PipelineState* m_PipelineStateObject;
		ID3D12RootSignature* m_RootSig;

		// Use our shader wrapper to handle shader files themselves
		ShaderDX12* m_VS = nullptr;
		ShaderDX12* m_PS = nullptr;

		// Our physical VB
		BufferDX12* m_VertexBuffer = nullptr;

		// Structure that points to VB in GPU
		D3D12_VERTEX_BUFFER_VIEW m_VertBufView;

		// Same for IB!
		BufferDX12* m_IndexBuffer = nullptr;
		D3D12_INDEX_BUFFER_VIEW m_IdxBufView;

		// Draw bounds stuff
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_Scissor;

		bool m_bNeedsResize = false;

	};
};
