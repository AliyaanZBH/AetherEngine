//===============================================================================
// desc: The DX11 engine, handles everything backend related. 
//		 Based upon one I built during a university project, but the goal is to optimize and modernize things going forward.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <vector>


#include "RendererDX11.h"
#include "D3DUtils.h"
//===============================================================================

bool RendererDX11::Initialize(IWindow& window)
{
	if (!CreateDevice())
		assert(false, "Failed to initialize DirectX 11 device.");
	// Retrieve the native window handle (HWND on Windows)

	HWND hwnd = static_cast<HWND>(window.GetNativeWindowHandle());

	// Create swapchain description based on the current window
	DXGI_SWAP_CHAIN_DESC sd;
	CreateSwapChainDescription(sd, hwnd, true, window.GetData().m_ClientWidth, window.GetData().m_ClientHeight);

    if (!CreateSwapChain(sd))
		assert(false, "Failed to initialize DirectX 11 swap chain.");

	// Needs to be executed every time the window is resized
	// So just call the OnResize method here to avoid code duplication.
	OnResize(window.GetData().m_ClientWidth, window.GetData().m_ClientHeight, *this);

	// Set sampler state
	CreateWrapSampler(m_pWrapSampler);

	// Return true if we made all the way here without failing previous functions
    return true;
}

void RendererDX11::Render()
{
	// Clear the back buffer
	float clearColor[4] = { 0.1f, 0.2f, 0.3f, 1.0f };
	m_pD3DImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);

	// Present the frame
	m_pSwapChain->Present(1, 0);
}

void RendererDX11::Terminate()
{
	// Clean up D3D and exit safely
	// 
	// Check if full screen - not advisable to exit in full screen mode
	if (m_pSwapChain)
	{
		BOOL fullscreen = false;
		HR(m_pSwapChain->GetFullscreenState(&fullscreen, nullptr));
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
	HR(m_pD3DDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pD3DDebug)));
	HR(pD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY));
}


bool RendererDX11::CreateDevice()
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
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory));
	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vAdapters.push_back(pAdapter);
		DXGI_ADAPTER_DESC desc;
		HR(pAdapter->GetDesc(&desc));
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
	HR(D3D11CreateDevice(
		vAdapters[useIdx],					// Default adapter
		m_D3DDriverType,
		0,									// No software device
		creationFlags,
		0, 0,								// Default feature level array
		D3D11_SDK_VERSION,
		&m_pD3DDevice,
		NULL,
		&m_pD3DImmediateContext));

    return true;
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

bool RendererDX11::CreateSwapChain(DXGI_SWAP_CHAIN_DESC& sd)
{
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = 0;
	HR(m_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(m_pD3DDevice.Get(), &sd, &m_pSwapChain));

	return true;
}

bool RendererDX11::CreateRenderTargets()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

	HR(m_pD3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_pRenderTargetView));


	m_pD3DImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
	return true;
}

void RendererDX11::OnResize_Default(int clientWidth, int clientHeight)
{
	assert(m_pD3DImmediateContext);
	assert(m_pD3DDevice);
	assert(m_pSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	m_pRenderTargetView.Reset();
	//m_pDepthStencilView.Reset();
	//m_pDepthStencilBuffer.Reset();

	// Resize swap chain
	HR(m_pSwapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	// Create new render targets
	CreateRenderTargets();

	//D3D11_TEXTURE2D_DESC depthStencilDesc;
	//CreateDepthStencilDescription(depthStencilDesc, clientWidth, clientHeight, mEnable4xMsaa, m4xMsaaQuality - 1);
	//CreateDepthStencilBufferAndView(depthStencilDesc);
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
	HR(m_pD3DDevice->CreateSamplerState(&sampDesc, &pSampler));
}