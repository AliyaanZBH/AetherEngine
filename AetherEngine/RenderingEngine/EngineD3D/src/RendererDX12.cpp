#ifdef USE_DX12
//===============================================================================
// desc: The DX12 engine, handles everything backend related. 
//		 Based upon one built for a short challenge, but the goal is to optimize and modernize things going forward.
//
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererDX12.h"

#include "D3DUtils.h"

#include <d3dcompiler.h>
#include <d3dx12.h>

//===============================================================================

AETHER_RESULT RendererDX12::Initialize(IWindow& window)
{
	AETHER_RESULT ar = AETHER_OK;

	AETHER_ASSERT(CreateDevice());

	// Retrieve the native window handle (HWND on Windows) and data about the window for our swapchain
	HWND hwnd = static_cast<HWND>(window.GetNativeWindowHandle());
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
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

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

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();


	//
	//
	//  RTV Descriptor Heaps
	// 
	//


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

	// Start with the heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	AETHER_HR_ASSERT(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSBHeap)));
	m_DSBHeap->SetName(L"DSB Heap");

	// Now the view
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	// Optimised clear value struct
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	// Now finalise resource and create the object!
	CD3DX12_HEAP_PROPERTIES defaultHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_WinData.m_ClientWidth, m_WinData.m_ClientHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
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

	//
	//
	//
	//  Finally, fence setup
	//
	//

	for (int i = 0; i < m_kNumFrameBuffers; ++i)
	{
		AETHER_HR_ASSERT(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence[i])));

		// Init fences to 0
		m_FenceValue[i] = 0;
	}

	// Now create event, we can re-use this for both fences
	m_FenceEvent = CreateEvent(NULL, false, false, NULL);
	if (m_FenceEvent == nullptr)
		return AETHER_FAIL;


	// Create pipelines!
	AETHER_ASSERT(CreatePipelines());

	// Return result - if we made all the way here without failing previous functions this should be A-OK!
	return ar;
}

void RendererDX12::Render()
{
	AETHER_RESULT ar = AETHER_OK;

	// Begin every render by first clearing the frame
	AETHER_ASSERT(ClearFrame());

	// Draw something! Simple triangle for now

	// draw triangle
	m_CmdList->SetGraphicsRootSignature(m_RootSig);                             // Set the root signature
	m_CmdList->RSSetViewports(1, &m_Viewport);                                  // Set the viewports
	m_CmdList->RSSetScissorRects(1, &m_Scissor);                                // Set the scissor rects
	m_CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);     // Set the primitive topology
	m_CmdList->IASetVertexBuffers(0, 1, &m_VertBufView);                        // Set the vertex buffer (using the vertex buffer view)
	// m_CmdList->DrawInstanced(3, 1, 0, 0);                                    // Finally draw 3 vertices (draw the triangle)
	m_CmdList->IASetIndexBuffer(&m_IdxBufView);                                 // Set IB
	m_CmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);                             // Draw 2 triangles (draw 1 instance of 2 triangles)
	m_CmdList->DrawIndexedInstanced(6, 1, 0, 4, 0);                             // Draw second quad

	// Now we've finished drawing, get the RT ready to present again. 
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CmdList->ResourceBarrier(1, &barrier);

	// Close cmdlist
	AETHER_HR_ASSERT(m_CmdList->Close());

	// Create array of cmd lists ( only using one atm tho)
	ID3D12CommandList* ppCmdLists[] = { m_CmdList.Get() };

	// Execute them
	m_CmdQueue->ExecuteCommandLists(1, ppCmdLists);

	// Signal our fence to help with syncing!
	AETHER_HR_ASSERT(m_CmdQueue->Signal(m_Fence[m_FrameIndex].Get(), m_FenceValue[m_FrameIndex]));

	// Backbuffer is ready to present, show us that frame!
	AETHER_HR_ASSERT(m_SwapChain->Present(0, 0));
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
	for (int i = 0; i < m_kNumFrameBuffers; ++i)
	{
		m_FrameIndex = i;
		Sync();
	}

	// Release and then delete everything
	m_Device->Release();
	m_Device = nullptr;
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

	// Last bit of additional setup - define our viewport and scissor rects
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

	return ar;
}

AETHER_RESULT RendererDX12::CompileShaders()
{
	AETHER_RESULT ar = AETHER_OK;

	ID3DBlob* vertexShader; // D3D blob for holding vertex shader bytecode
	ID3DBlob* errorBuff;    // A buffer holding the error data if any
	// [AZB]: Define the list of directories to search for include files
	std::vector<std::wstring> includeDirs =
	{
		L"..\\Shaders"  // [AZB]: Main common shader directory, this is where my stuff lives
	};

	// [AZB]: Create an instance of the custom include handler with the list of directories and the current shader
	std::wstring shaderFile = L"VertexShader.hlsl";
	CustomIncludeHandler includeHandler(includeDirs, shaderFile);

	std::wstring workingShaderDir = includeHandler.GetDirectories().back();
	std::wstring fullPathToShader = workingShaderDir + L"\\" + shaderFile;

	D3DCompileFromFile(fullPathToShader.c_str(),
		nullptr,
		&includeHandler,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShader,
		&errorBuff
	);
	
	// Print errors!
	if (errorBuff != nullptr)
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());

	// Fill out  shader bytecode struct, which is basically just a pointer to the shader bytecode and the size of the shader bytecode    
	m_VS.BytecodeLength = vertexShader->GetBufferSize();
	m_VS.pShaderBytecode = vertexShader->GetBufferPointer();

	// Repeat for pixel
	ID3DBlob* pixelShader; 
	shaderFile = L"PixelShader.hlsl";
	fullPathToShader = workingShaderDir + L"\\" + shaderFile;
	D3DCompileFromFile(fullPathToShader.c_str(),
		nullptr,
		&includeHandler,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShader,
		&errorBuff
	);
	if (errorBuff != nullptr)
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());

	m_PS.BytecodeLength = pixelShader->GetBufferSize();
	m_PS.pShaderBytecode = pixelShader->GetBufferPointer();

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
	psoDesc.VS = m_VS;
	psoDesc.PS = m_PS;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = m_SampleDesc;                                  // Same sample desc as swapchain
	psoDesc.SampleMask = 0xf;                                           // Point sampling
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);   // Lazy default init, good enough for triangle!
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);             // Lazy default init, good enough for triangle!
	psoDesc.DepthStencilState = dsDesc;                                 // Depth buffer enable!
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

	int vBufferSize = sizeof(verts);

	// Create default memory on GPU that we can copy into from an upload heap
	//
	// Use some default helpers that we can plug in as params
	CD3DX12_HEAP_PROPERTIES defaultHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
	AETHER_HR_ASSERT(m_Device->CreateCommittedResource(
		&defaultHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&defaultResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // Start in copy state as we are copying from the upload heap to this heap
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer)
	));

	m_VertexBuffer->SetName(L"Simple AHH Vert Buffer");

	// Create the upload heap now, GPU can read and CPU can write
	ID3D12Resource* vBufferUploadHeap;
	CD3DX12_HEAP_PROPERTIES uploadHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	AETHER_HR_ASSERT(m_Device->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&defaultResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap we created aboce
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap)
	));

	vBufferUploadHeap->SetName(L"CPU Vertex Buffer Upload Heap");

	// Store the vb data in the upload heap
	D3D12_SUBRESOURCE_DATA vertData = {};
	vertData.pData = (BYTE*)(verts);
	vertData.RowPitch = vBufferSize;
	vertData.SlicePitch = vBufferSize;

	// Create Command to copy the data across
	UpdateSubresources(m_CmdList.Get(), m_VertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertData);

	// Transition the vertex buffer data from copy destination state to vertex buffer state
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	m_CmdList->ResourceBarrier(1, &barrier);

	// Repeat for indices
	int iBufferSize = sizeof(indices);
	defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(iBufferSize);
	AETHER_HR_ASSERT(m_Device->CreateCommittedResource(
		&defaultHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&defaultResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_IndexBuffer)
	));
	
	m_IndexBuffer->SetName(L"Simple AHH Index Buffer");

	ID3D12Resource* iBufferUploadHeap;
	AETHER_HR_ASSERT(m_Device->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&defaultResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&iBufferUploadHeap)
	));
	
	iBufferUploadHeap->SetName(L"CPU Index Buffer Upload Heap");

	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = (BYTE*)(indices);
	indexData.RowPitch = iBufferSize;
	indexData.SlicePitch = iBufferSize;
	UpdateSubresources(m_CmdList.Get(), m_IndexBuffer.Get(), iBufferUploadHeap, 0, 0, 1, &indexData);
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	// Execute immediately here to send our geo buffers up one time
	m_CmdList->Close();
	ID3D12CommandList* ppCmdLists[] = { m_CmdList.Get()};
	m_CmdQueue->ExecuteCommandLists(_countof(ppCmdLists), ppCmdLists);

	// Update fence value and signal here too
	m_FenceValue[m_FrameIndex]++;
	AETHER_HR_ASSERT(m_CmdQueue->Signal(m_Fence[m_FrameIndex].Get(), m_FenceValue[m_FrameIndex]));


	// Finally, create VB and IB views for geo.
	m_VertBufView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertBufView.StrideInBytes = sizeof(Vertex);
	m_VertBufView.SizeInBytes = vBufferSize;

	m_CmdList->ResourceBarrier(1, &barrier);
	m_IdxBufView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IdxBufView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	m_IdxBufView.SizeInBytes = iBufferSize;

	return ar;
}

AETHER_RESULT RendererDX12::ClearFrame()
{
	AETHER_RESULT ar = AETHER_OK;

	// Make sure the command list is free before resetting
	Sync();

	// Clear allocator
	AETHER_HR_ASSERT(m_CmdAllocators[m_FrameIndex]->Reset());

	// Clear list and prep for recording.
	AETHER_HR_ASSERT(m_CmdList->Reset(m_CmdAllocators[m_FrameIndex].Get(), m_PipelineStateObject));

	// Begin recording - we're just clearing the frame for now
	//

	// Transition current backbuffer RT into state ready for drawing onto (or outputting onto, technically)
	//
	// Get a temp barrier to safely transition
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CmdList->ResourceBarrier(1, &barrier);

	// Get handle to RTV this frame, so that we can set it as the RT
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RTVDescripterSize);


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

AETHER_RESULT RendererDX12::Sync()
{
	AETHER_RESULT ar = AETHER_OK;

	// Swap index for our backbuffer
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// Check fenceValue - it will have updated if the GPU has finished executing
	if (m_Fence[m_FrameIndex]->GetCompletedValue() < m_FenceValue[m_FrameIndex])
	{
		// Create the fence event for when the value updates
		AETHER_HR_ASSERT(m_Fence[m_FrameIndex]->SetEventOnCompletion(m_FenceValue[m_FrameIndex], m_FenceEvent));

		// Wait here until the event is triggered
		WaitForSingleObject(m_FenceEvent, INFINITE);

	}

	// Increment for next frame
	m_FenceValue[m_FrameIndex]++;
	return ar;
}
#endif
