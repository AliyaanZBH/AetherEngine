#ifdef USE_DX11
//===============================================================================
// desc: The DX11 engine, handles everything backend related. 
//		 Based upon one I built during a university project, but the goal is to optimize and modernize things going forward.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "RendererDX11.h"
#include "D3DUtils.h"
#include "Pipeline.h" 
#include "DrawCommand.h"
//===============================================================================

namespace Aether
{
	AETHER_RESULT RendererDX11::Initialize(const IWindow& window)
	{
		AETHER_RESULT ar = AETHER_OK;

		AETHER_ASSERT(CreateDevice(), "Failed to initialize DirectX 11 device.");

		// Retrieve the native window handle (HWND on Windows)
		HWND hwnd = static_cast<HWND>(window.GetWin32Handle());
		m_WinData = window.GetData();

		// Create swapchain description based on the current window
		DXGI_SWAP_CHAIN_DESC sd;
		CreateSwapChainDescription(sd, hwnd, true, m_WinData.m_ClientWidth, m_WinData.m_ClientHeight);
		AETHER_ASSERT(CreateSwapChain(sd), "Failed to initialize DirectX 11 swap chain.");

		// Needs to be executed every time the window is resized
		// So just call the OnResize method here to avoid code duplication.
		// This calls CreateRenderTargets etc.
		OnResize(m_WinData.m_ClientWidth, m_WinData.m_ClientHeight, *this);

		CreateRasteriserState();

		// Set sampler state
		CreateWrapSampler(m_pWrapSampler);

		UpdateViewportAndScissor();
		
		m_pD3DImmediateContext->RSSetViewports(1, &m_Viewport);
		m_pD3DImmediateContext->RSSetScissorRects(1, &m_Scissor);

		// Create a constant buffer for per draw data


		// Return true / positive result if we made all the way here without failing previous functions
		return ar;
	}

	ShaderDX11* RendererDX11::LoadShader(ShaderHandle handle)
	{
		// See if this shader was already compiled, return it if so
		auto it = m_ShaderCache.find(handle);
		if (it != m_ShaderCache.end())
			return it->second;

		// Doesn't exist yet, let's build it. This will fail if the shader hasn't yet been registered
		const ShaderDesc& desc = ShaderLibrary::Get().GetDesc(handle);

		// DX is weird and windows-y so it wants a wstring
		std::wstring windowsPath = ToWide(ResolveDirectXShaderPath(desc.m_Name));
		ShaderDX11* shader = new ShaderDX11(windowsPath, ShaderStageToHLSLCompilerString(desc.m_ShaderStage));

		// Register shader in DX12 cache
		m_ShaderCache[handle] = shader;

		return shader;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> RendererDX11::TranslateLayout(const VertexLayout& layout)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> outLayout;
		size_t layoutSize = layout.m_Attributes.size();
		outLayout.reserve(layoutSize);

		// Iterate through each attribute (position, colour, normal, etc) and push back into our output layout
		for (uint32_t i = 0u; i < layoutSize; ++i)
		{
			const VertexAttribute& attr = layout.m_Attributes[i];

			D3D11_INPUT_ELEMENT_DESC desc
			{
				.SemanticName = ToDirectXSemantic(attr.m_Name),
				.SemanticIndex = 0,
				.Format = ToDXGIFormat(attr.m_Format),
				.InputSlot = 0,
				.AlignedByteOffset = attr.m_Offset,
				.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			};

			outLayout.push_back(desc);
		}

		return outLayout;
	}

	void RendererDX11::CreatePipeline(const PipelineDesc& desc)
	{
		// Load shaders into temp objects
		ShaderDX11* vs = LoadShader(desc.m_VertexShader);
		ShaderDX11* ps = LoadShader(desc.m_PixelShader);

		// Actually create specific shader from the binary blobs
		m_pD3DDevice->CreateVertexShader(vs->Get()->GetBufferPointer(), vs->Get()->GetBufferSize(), nullptr, &m_VS);
		m_pD3DDevice->CreatePixelShader(ps->Get()->GetBufferPointer(), ps->Get()->GetBufferSize(), nullptr, &m_PS);

		// Translate and create the input layout for our device
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputs = TranslateLayout(desc.m_Layout);
		m_pD3DDevice->CreateInputLayout(inputs.data(), UINT(inputs.size()), vs->Get()->GetBufferPointer(), vs->Get()->GetBufferSize(), &m_Layout);
		m_pD3DImmediateContext->IASetInputLayout(m_Layout);

		// Delete danglers
		delete vs;
		delete ps;

	}

	void RendererDX11::Render()
	{} // Currently not rendering any internal geometry

	void RendererDX11::Submit(const DrawCommand& cmd, ConstantBufferView* cbv)
	{
		//uint32_t cbSlot = cbv->m_Slot;
		//ID3D11Buffer* conBuf = static_cast<BufferDX11*>(cbv->m_Buffer)->GetBuffer();

		// Set constant buffer
		//m_pD3DImmediateContext->VSSetConstantBuffers(cbSlot, 1, &conBuf);
		//m_pD3DImmediateContext->PSSetConstantBuffers(cbSlot, 1, &conBuf);

		// DX11 is immediate mode so we can render immediately
		Render(cmd.m_VBV, cmd.m_IBV);
	}

	void RendererDX11::Render(VertexBufferView* vbv, IndexBufferView* ibv)
	{
		// Grab geo buffers
		BufferDX11* vertBuf = static_cast<BufferDX11*>(vbv->m_Buffer);
		BufferDX11* indexBuf = static_cast<BufferDX11*>(ibv->m_Buffer);

		ID3D11Buffer* dxVB = vertBuf->GetBuffer();
		ID3D11Buffer* dxIB = indexBuf->GetBuffer();

		UINT offset = vbv->m_Offset;

		m_pD3DImmediateContext->IASetInputLayout(m_Layout);
		m_pD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pD3DImmediateContext->IASetVertexBuffers(0, 1, &dxVB, &vbv->m_Stride, &offset);
		m_pD3DImmediateContext->IASetIndexBuffer(dxIB, DXGI_FORMAT_R32_UINT, 0);
	
		m_pD3DImmediateContext->VSSetShader(m_VS, nullptr, 0);
		m_pD3DImmediateContext->PSSetShader(m_PS, nullptr, 0);

		m_pD3DImmediateContext->RSSetViewports(1, &m_Viewport);
		m_pD3DImmediateContext->RSSetScissorRects(1, &m_Scissor);
		m_pD3DImmediateContext->RSSetState(m_pRasterState.Get());

		m_pD3DImmediateContext->DrawIndexed(ibv->m_Count, 0, 0);

	}

	void RendererDX11::Present()
	{
		m_pSwapChain->Present(1, 0);
	}

	void RendererDX11::Resize(int newWidth, int newHeight)
	{
		OnResize_Default(newWidth, newHeight);
	}

	void RendererDX11::ClearFrame()
	{

		// Clear the back buffer
		float clearColor[4] = { 1.f, 0.3f, 0.0f, 1.0f };
		m_pD3DImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);

		//m_pD3DImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
		// Set render target ready for drawing
		m_pD3DImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(),nullptr);

		// Clear depth aswell!
		m_pD3DImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		UpdateViewportAndScissor();
	}

	void RendererDX11::Terminate()
	{
		// Clean up D3D and exit safely
		// 
		// Check if full screen - not advisable to exit in full screen mode
		if (m_pSwapChain)
		{
			BOOL fullscreen = false;
			AETHER_HR_ASSERT(m_pSwapChain->GetFullscreenState(&fullscreen, nullptr));
			if (fullscreen) // Go for a window
				m_pSwapChain->SetFullscreenState(false, nullptr);
		}

		// Clear context
		if (m_pD3DImmediateContext)
		{
			m_pD3DImmediateContext->ClearState();
			m_pD3DImmediateContext->Flush();
		}

		// Some extra reporting
		ID3D11Debug* pD3DDebug;
		AETHER_HR_ASSERT(m_pD3DDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pD3DDebug)));
		AETHER_HR_ASSERT(pD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY));
	}

	Buffer* RendererDX11::CreateBuffer(const BufferDesc& desc)
	{
		return new BufferDX11(desc, m_pD3DDevice.Get(), m_pD3DImmediateContext.Get());
	}

	void RendererDX11::InitImGui()
	{
		ImGui_ImplDX11_Init(m_pD3DDevice.Get(), m_pD3DImmediateContext.Get());
	}

	void RendererDX11::BeginImGuiRender()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void RendererDX11::EndImGuiRender()
	{
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	AETHER_RESULT RendererDX11::CreateDevice()
	{

		// Setup to enable debug layer
		UINT creationFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		m_D3DDriverType = D3D_DRIVER_TYPE_UNKNOWN;

		// Figure out how many gpus we have
		IDXGIAdapter* pAdapter;
		std::vector<IDXGIAdapter*> vAdapters;
		// Factories handle full screen transitions
		IDXGIFactory* pFactory = NULL;
		SIZE_T useIdx = -1, mostRam = -1;
		AETHER_HR_ASSERT(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory));
		for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			vAdapters.push_back(pAdapter);
			DXGI_ADAPTER_DESC desc;
			AETHER_HR_ASSERT(pAdapter->GetDesc(&desc));
			//WDBOUT(L"Found adapter=(" << i << ") " << desc.Description << L" VRAM=" << desc.DedicatedVideoMemory);
			if (desc.DedicatedVideoMemory > mostRam || mostRam == -1)
			{
				useIdx = i;
				mostRam = desc.DedicatedVideoMemory;
			}
		}
		if (pFactory)
			pFactory->Release();
		assert(useIdx >= 0);

		//D3D_FEATURE_LEVEL featureLevel;
		AETHER_HR_ASSERT(D3D11CreateDevice(
			vAdapters[useIdx],					// Default adapter
			m_D3DDriverType,
			0,									// No software device
			creationFlags,
			0, 0,								// Default feature level array
			D3D11_SDK_VERSION,
			&m_pD3DDevice,
			NULL,
			&m_pD3DImmediateContext));

		return AETHER_OK;
	}

	void RendererDX11::CreateSwapChainDescription(DXGI_SWAP_CHAIN_DESC& sd, HWND hMainWnd, bool windowed, int screenWidth, int screenHeight)
	{
		// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
		sd.BufferDesc.Width = screenWidth;
		sd.BufferDesc.Height = screenHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = hMainWnd;
		sd.Windowed = windowed;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		// MSAA
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;

		m_Windowed = windowed;

	}

	AETHER_RESULT RendererDX11::CreateSwapChain(DXGI_SWAP_CHAIN_DESC& sd)
	{
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = 0;
		AETHER_HR_ASSERT(m_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

		Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter = 0;
		AETHER_HR_ASSERT(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

		Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory = 0;
		AETHER_HR_ASSERT(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

		AETHER_HR_ASSERT(dxgiFactory->CreateSwapChain(m_pD3DDevice.Get(), &sd, &m_pSwapChain));

		return AETHER_OK;
	}

	AETHER_RESULT RendererDX11::CreateRenderTargets()
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		AETHER_HR_ASSERT(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

		AETHER_HR_ASSERT(m_pD3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

		m_pD3DImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
		return AETHER_OK;
	}

	AETHER_RESULT RendererDX11::CreateRasteriserState()
	{
		D3D11_RASTERIZER_DESC rsDesc = {};
		rsDesc.FillMode = D3D11_FILL_SOLID;
		//rsDesc.CullMode = D3D11_CULL_BACK;
		rsDesc.CullMode = D3D11_CULL_NONE;
		//rsDesc.DepthClipEnable = TRUE;
		rsDesc.DepthClipEnable = FALSE;

		m_pD3DDevice->CreateRasterizerState(&rsDesc, &m_pRasterState);
		m_pD3DImmediateContext->RSSetState(m_pRasterState.Get());

		return AETHER_OK;
	}

	// Create the depth/stencil buffer description
	void RendererDX11::CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& dsd, int screenWidth, int screenHeight, bool msaa, int count, int maxQuality)
	{
		dsd.Width = screenWidth;
		dsd.Height = screenHeight;
		dsd.MipLevels = 1;
		dsd.ArraySize = 1;
		dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// Use MSAA? -- must match swap chain MSAA values.
		if (msaa)
		{
			dsd.SampleDesc.Count = count;
			dsd.SampleDesc.Quality = maxQuality - 1;
		}
		// No MSAA
		else
		{
			dsd.SampleDesc.Count = 1;
			dsd.SampleDesc.Quality = 0;
		}

		dsd.Usage = D3D11_USAGE_DEFAULT;
		dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsd.CPUAccessFlags = 0;
		dsd.MiscFlags = 0;
	}

	AETHER_RESULT RendererDX11::CreateDepthStencilBufferAndView(D3D11_TEXTURE2D_DESC& dsd)
	{
		AETHER_HR_ASSERT(m_pD3DDevice->CreateTexture2D(&dsd, 0, m_pDepthStencilBuffer.GetAddressOf()));
		AETHER_HR_ASSERT(m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), 0, m_pDepthStencilView.GetAddressOf()));
		return AETHER_OK;
	}

	void RendererDX11::OnResize_Default(int clientWidth, int clientHeight)
	{
		assert(m_pD3DImmediateContext);
		assert(m_pD3DDevice);
		assert(m_pSwapChain);
		
		// Release the old views, as they hold references to the buffers we
		// will be destroying.  Also release the old depth/stencil buffer.
		
		m_pRenderTargetView.Reset();
		m_pDepthStencilView.Reset();
		m_pDepthStencilBuffer.Reset();
		
		// Resize swap chain
		AETHER_HR_ASSERT(m_pSwapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		
		// Create depth stencil
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		
		// MSAA currently disabled
		CreateDepthStencilDescription(depthStencilDesc, clientWidth, clientHeight, false, 2, 1);
		AETHER_ASSERT(CreateDepthStencilBufferAndView(depthStencilDesc), "Failed to create depth stencil buffer and view in DX11");
		
		// Create new render targets
		CreateRenderTargets();
		
		// Update windata struct and then update viewport
		m_WinData.m_ClientWidth = clientWidth;
		m_WinData.m_ClientHeight = clientHeight;
		
		UpdateViewportAndScissor();
	}

	void RendererDX11::CreateWrapSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState>& pSampler)
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		AETHER_HR_ASSERT(m_pD3DDevice->CreateSamplerState(&sampDesc, &pSampler));
	}

	void RendererDX11::UpdateViewportAndScissor()
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

		m_pD3DImmediateContext->RSSetViewports(1, &m_Viewport);
		m_pD3DImmediateContext->RSSetScissorRects(1, &m_Scissor);
	}
};
#endif