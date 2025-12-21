#ifdef USE_DX12
//===============================================================================
// desc: The DX12 engine, handles everything backend related. 
//		 Based upon one built for a short challenge, but the goal is to optimize and modernize things going forward.
//
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererDX12.h"
#include "D3DUtils.h"
//===============================================================================
namespace Aether
{
	AETHER_RESULT RendererDX12::Initialize(IWindow& window)
	{
		AETHER_RESULT ar = AETHER_OK;

		AETHER_ASSERT(CreateDevice());

		// Retrieve the native window handle (HWND on Windows) and data about the window for our swapchain
		HWND hwnd = static_cast<HWND>(window.GetWin32Handle());
		m_WinData = window.GetData();


		//
		//
		//  Create command queue first, will need this for our swapchain
		//
		//


		D3D12_COMMAND_QUEUE_DESC cqDesc = {}; // Use default values for now
		AETHER_HR_ASSERT(m_Device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_CmdQueue))); // Create the command queue


		//
		//
		// Swap chain time!
		//
		//


		// Pinch values from our window!
		DXGI_MODE_DESC backBufferDesc = {};
		backBufferDesc.Width = m_WinData.m_ClientWidth;
		backBufferDesc.Height = m_WinData.m_ClientHeight;
		backBufferDesc.Format = m_kRTVFormat; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

		// We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
		m_SampleDesc.Count = 1;

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = m_kNumFrameBuffers;
		swapChainDesc.BufferDesc = backBufferDesc;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// dxgi will discard the buffer (data) after we call present
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc = m_SampleDesc;						// our multi-sampling description
		swapChainDesc.Windowed = !m_WinData.m_bFullscreen;				// set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

		IDXGISwapChain* tempSwapChain;

		m_DXGIFactory->CreateSwapChain(
			m_CmdQueue.Get(),
			&swapChainDesc,
			&tempSwapChain
		);

		m_SwapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

		m_FrameContextIndex = m_SwapChain->GetCurrentBackBufferIndex();


		//
		//
		//  RTV Descriptor Heaps and Targets themselves
		// 
		//

		// Re-usable helper function to create the RTVs
		AETHER_ASSERT(CreateRenderTargets());
		 

		//
		//
		//  SRV Descriptor Heaps
		// 
		//

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.NumDescriptors = m_kSRVHeapSize;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		AETHER_HR_ASSERT(m_Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SRVHeap)));
        m_SRVHeapAllocator.Create(m_Device.Get(), m_SRVHeap.Get());
    


		//
		// Command Allocators
		//

		for (int i = 0; i < m_kNumFrameBuffers; ++i)
		{
			AETHER_HR_ASSERT(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CmdAllocators[i])));
		}

		//
		//  Command List itself!
		//

		AETHER_HR_ASSERT(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CmdAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_CmdList)));


		//
		//
		//  Create the depth stencil buffer!
		//
		//

		AETHER_ASSERT(CreateDepthStencil());

		//
		//
		//
		//  Finally, fence setup
		//
		//

		AETHER_HR_ASSERT(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		
		// Init fence values to 0
		m_FenceValue[0] = 0;
		m_FenceValue[1] = 0;

		// Now create event, we can re-use this for both fences
		m_FenceEvent = CreateEvent(NULL, false, false, NULL);
		if (m_FenceEvent == nullptr)
			return AETHER_FAIL;


		// Create pipelines!
		AETHER_ASSERT(CreatePipelines());

		// Return result - if we made all the way here without failing previous functions this should be A-OK!
		return ar;
	}

	void RendererDX12::InitImGui()
	{
		ImGui_ImplDX12_InitInfo init_info = {};
		init_info.Device = m_Device.Get();
		init_info.CommandQueue = m_CmdQueue.Get();
		init_info.NumFramesInFlight = m_kNumFrameBuffers;
		init_info.RTVFormat = m_kRTVFormat;
		init_info.DSVFormat = m_kDSVFormat;
		init_info.SrvDescriptorHeap = m_SRVHeap.Get();
		init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return m_SRVHeapAllocator.Alloc(out_cpu_handle, out_gpu_handle); };
		init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return m_SRVHeapAllocator.Free(cpu_handle, gpu_handle); };
		ImGui_ImplDX12_Init(&init_info);
	}

	void RendererDX12::BeginImGuiRender()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void RendererDX12::EndImGuiRender()
	{
		ImGui::Render();
		ID3D12DescriptorHeap* pSrvHeaps[] = { m_SRVHeap.Get() };
		m_CmdList->SetDescriptorHeaps(1, pSrvHeaps);

		// Render ImGui on top of everything else!
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CmdList.Get());
	}
	void RendererDX12::Render()
	{
		AETHER_RESULT ar = AETHER_OK;

		// Draw something! Simple depth tested quads for now
		m_CmdList->SetGraphicsRootSignature(m_RootSig);                             // Set the root signature
		m_CmdList->RSSetViewports(1, &m_Viewport);                                  // Set the viewports
		m_CmdList->RSSetScissorRects(1, &m_Scissor);                                // Set the scissor rects
		m_CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);     // Set the primitive topology
		m_CmdList->IASetVertexBuffers(0, 1, &m_VertBufView);                        // Set the vertex buffer (using the vertex buffer view)
		m_CmdList->IASetIndexBuffer(&m_IdxBufView);                                 // Set IB
		m_CmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);                             // Draw 2 triangles (draw 1 instance of 2 triangles)
		m_CmdList->DrawIndexedInstanced(6, 1, 0, 4, 0);                             // Draw second quad

	}

	void RendererDX12::Present()
	{
		// Now we've finished drawing, get the RT ready to present again. 
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_BackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_CmdList->ResourceBarrier(1, &barrier);

		// Close cmdlist
		AETHER_HR_ASSERT(m_CmdList->Close());

		// Create array of cmd lists ( only using one atm tho)
		ID3D12CommandList* ppCmdLists[] = { m_CmdList.Get() };

		// Execute them
		m_CmdQueue->ExecuteCommandLists(1, ppCmdLists);

		// Backbuffer is ready to present, show us that frame!
		AETHER_HR_ASSERT(m_SwapChain->Present(1, 0));

		// Signal fence for THIS CPU frame
		m_FenceValue[m_FrameContextIndex] = ++m_GlobalFenceValue;
		AETHER_HR_ASSERT(m_CmdQueue->Signal(m_Fence.Get(), m_FenceValue[m_FrameContextIndex]));
	}

	void RendererDX12::Resize(int newWidth, int newHeight)
	{
		// Update stored windata values and set flag so that we resize at a safe point in our render pipeline.
		m_WinData.m_ClientWidth = newWidth;
		m_WinData.m_ClientHeight = newHeight;
		m_bNeedsResize = true;
	}

	AETHER_RESULT RendererDX12::CreateRenderTargets()
	{

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = m_kNumFrameBuffers;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;        // Back buffer won't ever be shaded directly, so no need to be visible!

		AETHER_HR_ASSERT(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVHeap)));

		// Get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
		m_RTVDescripterSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Get handle to the first descriptor in the descriptor heap.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each buffer
		for (int i = 0; i < m_kNumFrameBuffers; ++i)
		{

			AETHER_HR_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));

			m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
			// Increment the rtv handle by the rtv descriptor size we got above
			rtvHandle.Offset(1, m_RTVDescripterSize);
		}

		return AETHER_OK;
	}

	AETHER_RESULT RendererDX12::CreateDepthStencil()
	{
		// Start with the heap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		AETHER_HR_ASSERT(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSBHeap)));
		m_DSBHeap->SetName(L"DSB Heap");

		// Now the view
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = m_kDSVFormat;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

		// Optimised clear value struct
		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		depthOptimizedClearValue.Format = m_kDSVFormat;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		// Now finalise resource and create the object!
		CD3DX12_HEAP_PROPERTIES defaultHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(m_kDSVFormat, m_WinData.m_ClientWidth, m_WinData.m_ClientHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		AETHER_HR_ASSERT(m_Device->CreateCommittedResource(
			&defaultHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&defaultResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&m_DepthStencilBuffer)
		));

		m_DepthStencilBuffer->SetName(L"Depth Stencil Buffer");

		// Now create view on device!
		m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &depthStencilDesc, m_DSBHeap->GetCPUDescriptorHandleForHeapStart());

		return AETHER_OK;
	}


	AETHER_RESULT RendererDX12::CleanupRenderBuffers()
	{
		// Reset current RT only
		//AETHER_HR_ASSERT(m_RenderTargets[m_FrameContextIndex].Reset());
		for (int i = 0; i < m_kNumFrameBuffers; ++i)
		{
			AETHER_HR_ASSERT(m_RenderTargets[i].Reset());
		}

		// Also reset the heap!
		AETHER_HR_ASSERT(m_RTVHeap.Reset());

		// Repeat for DSV and SRV
		AETHER_HR_ASSERT(m_DepthStencilBuffer.Reset());
		AETHER_HR_ASSERT(m_DSBHeap.Reset());

		return AETHER_OK;
	}

	AETHER_RESULT RendererDX12::UpdateViewportAndScissor()
	{
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = m_WinData.m_ClientWidth;
		m_Viewport.Height = m_WinData.m_ClientHeight;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_Scissor.left = 0;
		m_Scissor.top = 0;
		m_Scissor.right = m_WinData.m_ClientWidth;
		m_Scissor.bottom = m_WinData.m_ClientHeight;
		return AETHER_OK;
	}

	void RendererDX12::ClearFrame()
	{
		ClearAndSyncFrame();
	}

	void RendererDX12::Terminate()
	{
		// Clean up D3D and exit safely
		// 
		// Check if full screen - not advisable to exit in full screen mode
		if (m_SwapChain)
		{
			BOOL fullscreen = false;
			AETHER_HR_ASSERT(m_SwapChain->GetFullscreenState(&fullscreen, nullptr));
			if (fullscreen) // Go for a window
				m_SwapChain->SetFullscreenState(false, nullptr);
		}


		// Wait for GPU to safely finish final frames
		//for (int i = 0; i < m_kNumFrameBuffers; ++i)
		//{
		//	m_FrameContextIndex = i;
		//	Sync();
		//}

		// Release and then delete everything
		m_Device->Release();
		m_Device = nullptr;
	}

	Buffer* RendererDX12::CreateBuffer(const BufferDesc& desc)
	{
		return new BufferDX12(desc, m_Device.Get(), m_CmdList.Get());
	}


	AETHER_RESULT RendererDX12::CreateDevice()
	{
		AETHER_RESULT ar = AETHER_OK;

		//
		//
		// Create the Device - first we need to find the best hardware adapter, grab this via dxgi factory
		//
		//

		AETHER_HR_ASSERT(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DXGIFactory)));


		IDXGIAdapter1* adapter;
		int adapterIndex = 0;
		bool adapterFound = false;

		// Setup to enable debug layer
#if defined(DEBUG) || defined(_DEBUG)  

		ID3D12Debug* debugInterface;

		AETHER_HR_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));

		debugInterface->EnableDebugLayer();
#endif

		// Find first hardware GPU that supports d3d 12
		while (m_DXGIFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// We don't want a software device
				adapterIndex++;
				continue;
			}

			// We've found a GPU - Make sure it's a device that is compatible with direct3d 12 (feature level 11 or higher)
			ar = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
			if (SUCCEEDED(ar))
			{
				// Success! we made it!
				adapterFound = true;
				break;
			}

			adapterIndex++;
		}

		// Didn't find anything, return error code
		if (!adapterFound)
		{
			ar = AETHER_FAIL;
		}

		return ar;
	}

	AETHER_RESULT RendererDX12::CreatePipelines()
	{
		AETHER_RESULT ar = AETHER_OK;

		// Create root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootDesc = {};
		rootDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// For error checking
		ID3DBlob* signature;
		AETHER_HR_ASSERT(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr));

		AETHER_HR_ASSERT(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSig)));

		// Compile shaders for this pipeline!
		AETHER_ASSERT(CompileShaders());

		// Build PSO
		AETHER_ASSERT(CreateInputLayoutAndPSO());

		AETHER_ASSERT(CreateAndUploadGeo());

		// Last bit of additional setup - define our viewport and scissor rects. This needs re-calling on resize!
		UpdateViewportAndScissor();

		return ar;
	}

	AETHER_RESULT RendererDX12::CompileShaders()
	{
		AETHER_RESULT ar = AETHER_OK;
		m_VS = new ShaderDX12(L"VertexShader.hlsl", "vs_5_0");
		m_PS = new ShaderDX12(L"PixelShader.hlsl", "ps_5_0");
		return ar;
	}

	AETHER_RESULT RendererDX12::CreateInputLayoutAndPSO()
	{
		AETHER_RESULT ar = AETHER_OK;

		// Create input layout for our input assembler so it knows how to read our vert attributes
		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		D3D12_INPUT_LAYOUT_DESC inputDesc = {};

		// Simple (size of array) / (size of element type) to get num elements
		inputDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		inputDesc.pInputElementDescs = inputLayout;

		// Create a depth buffer - use a default one for now
		CD3DX12_DEPTH_STENCIL_DESC dsDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

		// Simple PSO for our humble geo
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = inputDesc;
		psoDesc.pRootSignature = m_RootSig;
		psoDesc.VS = m_VS->Get();
		psoDesc.PS = m_PS->Get();
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RTVFormats[0] = m_kRTVFormat;
		psoDesc.SampleDesc = m_SampleDesc;                                  // Same sample desc as swapchain
		psoDesc.SampleMask = 0xf;                                           // Point sampling
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);   // Lazy default init, good enough for triangle!
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);             // Lazy default init, good enough for triangle!
		psoDesc.DepthStencilState = dsDesc;                                 // Depth buffer enable!
		psoDesc.DSVFormat = m_kDSVFormat;									// Depth buffer format!
		psoDesc.NumRenderTargets = 1;

		// Create the PSO
		AETHER_HR_ASSERT(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineStateObject)));

		return ar;
	}

	AETHER_RESULT RendererDX12::CreateAndUploadGeo()
	{
		AETHER_RESULT ar = AETHER_OK;

		// More hardcoding, just send a triangle up
		   //
		   //Vertex verts[] =
		   //{
		   //    { 0.0f, 0.5f, 0.5f,     1.0f, 0.f, 0.f, 1.f },
		   //    { 0.5f, -0.5f, 0.5f,    0.f, 1.0f, 0.f, 1.f },
		   //    { -0.5f, -0.5f, 0.5f,   0.f, 0.f, 1.0f, 1.f }
		   //};


		// We got quads now baybee!
		Vertex verts[] =
		{
			// First Quad
			{ -0.5f,  0.5f, 0.5f,   1.0f, 0.f, 0.f, 1.f }, // Top left
			{  0.5f, -0.5f, 0.5f,   0.f, 1.0f, 0.f, 1.f }, // Bottom right
			{ -0.5f, -0.5f, 0.5f,   0.f, 0.f, 1.0f, 1.f }, // Bottom left
			{  0.5f,  0.5f, 0.5f,   1.f, 1.0f, 1.f, 1.f },  // Top right

			// Second Quad - flip colours
			{ -0.75f,  0.75f, 0.7f,   1.f, 1.0f, 1.f, 1.f }, // Top left
			{  0.0f, 0.0f, 0.7f,   0.f, 0.f, 1.0f, 1.f }, // Bottom right
			{ -0.75f, 0.0f, 0.7f,   0.f, 1.0f, 0.f, 1.f }, // Bottom left
			{  0.0f,  0.75f, 0.7f,   1.0f, 0.f, 0.f, 1.f }  // Top right
		};

		// Indices too!
		DWORD indices[] =
		{
			0, 1, 2, // first triangle
			0, 3, 1 // second triangle
		};


		//
		//  Init VBuffer
		//

		BufferDesc vbDesc =
		{
			.m_Data = verts,
			.m_SizeInBytes = sizeof(verts),
			.m_Type = eBufferType::kVertex,
			.m_CPUVisible = true
		};

		m_VertexBuffer = static_cast<BufferDX12*>(CreateBuffer(vbDesc));
		m_VertexBuffer->SetName(L"Simple AHH Vert Buffer");

		// Upload the buffer to the GPU now, the buffer helper will internally work out if this will map the resource directly or copy to the default heap of the GPU
		m_VertexBuffer->Upload(vbDesc.m_Data, vbDesc.m_SizeInBytes);
		
		// Transition the vertex buffer data from copy destination state to vertex buffer state
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer->GetResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		m_CmdList->ResourceBarrier(1, &barrier);

		// Repeat for indices
		BufferDesc ibDesc =
		{
			.m_Data = indices,
			.m_SizeInBytes = sizeof(indices),
			.m_Type = eBufferType::kIndex,
			.m_CPUVisible = true
		};

		m_IndexBuffer = static_cast<BufferDX12*>(CreateBuffer(ibDesc));
		m_IndexBuffer->SetName(L"Simple AHH Index Buffer");
		m_IndexBuffer->Upload(ibDesc.m_Data, ibDesc.m_SizeInBytes);
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_IndexBuffer->GetResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		m_CmdList->ResourceBarrier(1, &barrier);

		// Execute immediately here to send our geo buffers up one time
		m_CmdList->Close();
		ID3D12CommandList* ppCmdLists[] = { m_CmdList.Get() };
		m_CmdQueue->ExecuteCommandLists(1, ppCmdLists);

		// Signal once
		const UINT64 uploadFence = ++m_GlobalFenceValue;
		AETHER_HR_ASSERT(m_CmdQueue->Signal(m_Fence.Get(), uploadFence));

		// Wait once
		AETHER_HR_ASSERT(m_Fence->SetEventOnCompletion(uploadFence, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);

		// Finally, create VB and IB views for geo.
		m_VertBufView.BufferLocation = m_VertexBuffer->GetResource()->GetGPUVirtualAddress();
		m_VertBufView.StrideInBytes = sizeof(Vertex);
		m_VertBufView.SizeInBytes = vbDesc.m_SizeInBytes;

		m_IdxBufView.BufferLocation = m_IndexBuffer->GetResource()->GetGPUVirtualAddress();
		m_IdxBufView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
		m_IdxBufView.SizeInBytes = ibDesc.m_SizeInBytes;

		return ar;
	}

	AETHER_RESULT RendererDX12::ClearAndSyncFrame()
	{
		AETHER_RESULT ar = AETHER_OK;

		// Make sure the command list is free before resetting
		BeginFrame();

		// Check if we need to resize!
		if (m_bNeedsResize)
		{
			// Wait for GPU to properly flush, it may still be processing the last frame and hasn't flipped yet
			WaitForGPU();

			// Cleanly release all current buffers
			CleanupRenderBuffers();

			// Actually rezie the swapchain
			DXGI_SWAP_CHAIN_DESC1 desc = {};
			m_SwapChain->GetDesc1(&desc);
			AETHER_HR_ASSERT(m_SwapChain->ResizeBuffers(m_kNumFrameBuffers, m_WinData.m_ClientWidth, m_WinData.m_ClientHeight, desc.Format, desc.Flags));

			// Rebuild buffers at this new size
			AETHER_ASSERT(CreateRenderTargets());
			AETHER_ASSERT(CreateDepthStencil());

			m_bNeedsResize = false;
		}

		// Clear allocator
		AETHER_HR_ASSERT(m_CmdAllocators[m_FrameContextIndex]->Reset());

		// Clear list and prep for recording.
		AETHER_HR_ASSERT(m_CmdList->Reset(m_CmdAllocators[m_FrameContextIndex].Get(), m_PipelineStateObject));


		// Begin recording - we're just clearing the frame for now
		//

		// Transition current backbuffer RT into state ready for drawing onto (or outputting onto, technically)
		m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
		// Get a temp barrier to safely transition
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_BackBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_CmdList->ResourceBarrier(1, &barrier);


		// Get handle to RTV this frame, so that we can set it as the RT
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_BackBufferIndex, m_RTVDescripterSize);

		// Get our DSB!
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsbHandle(m_DSBHeap->GetCPUDescriptorHandleForHeapStart());

		// Actually set RT now - using our depth stencil buffer!
		m_CmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsbHandle);

		// Clear our RT
		m_CmdList->ClearRenderTargetView(rtvHandle, m_kClearColour, 0, nullptr);

		// Clear DSB too
		m_CmdList->ClearDepthStencilView(m_DSBHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// Continue drawing
		return ar;
	}

	AETHER_RESULT RendererDX12::BeginFrame()
	{
		AETHER_RESULT ar = AETHER_OK;

		// Advance CPU frame context
		m_FrameContextIndex = (m_FrameContextIndex + 1) % m_kNumFrameBuffers;

		// Check fenceValue - it will have updated if the GPU has finished executing
		if (m_Fence->GetCompletedValue() < m_FenceValue[m_FrameContextIndex])
		{
			// Create the fence event for when the value updates
			AETHER_HR_ASSERT(m_Fence->SetEventOnCompletion(m_FenceValue[m_FrameContextIndex], m_FenceEvent));

			// Wait here until the event is triggered
			WaitForSingleObject(m_FenceEvent, INFINITE);

		}

		return ar;
	}

	AETHER_RESULT RendererDX12::WaitForGPU()
	{
		const UINT64 fence = ++m_GlobalFenceValue;
		m_CmdQueue->Signal(m_Fence.Get(), fence);
		AETHER_HR_ASSERT(m_Fence->SetEventOnCompletion(fence, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
		return AETHER_OK;
	}
};
#endif
