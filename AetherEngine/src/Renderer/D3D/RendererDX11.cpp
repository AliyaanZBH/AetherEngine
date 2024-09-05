//===============================================================================
// desc: The DX11 engine, handles everything backend related. 
//		 Based upon one I built during a university project, but the goal is to optimize and modernize things going forward.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <dxgi1_3.h>
#include <d3dcompiler.h>

#include "RendererDX11.h"
#include "D3DUtil.h"
//===============================================================================


bool RendererDX11::Initialize()
{
    if (!CreateDevice()) {
        std::cerr << "Failed to initialize DirectX 11 device." << std::endl;
        return false;
    }
    if (!CreateSwapChain()) {
        std::cerr << "Failed to initialize DirectX 11 swap chain." << std::endl;
        return false;
    }
    if (!InitRenderTarget()) {
        std::cerr << "Failed to initialize DirectX 11 render target." << std::endl;
        return false;
    }
    return true;
}


bool RendererDX11::CreateDevice()
{

	// Setup to enable debug layer
	UINT creationFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	md3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;

	// Figure out how many gpus we have
	IDXGIAdapter* pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;
	// Factories handle full screen transitions
	IDXGIFactory* pFactory = NULL;
	SIZE_T useIdx = -1, mostRam = -1;
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory));
	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vAdapters.push_back(pAdapter);
		DXGI_ADAPTER_DESC desc;
		HR(pAdapter->GetDesc(&desc));
		WDBOUT(L"Found adapter=(" << i << ") " << desc.Description << L" VRAM=" << desc.DedicatedVideoMemory);
		if (desc.DedicatedVideoMemory > mostRam || mostRam == -1)
		{
			useIdx = i;
			mostRam = desc.DedicatedVideoMemory;
		}
	}
	if (pFactory)
		pFactory->Release();
	assert(useIdx >= 0);

	D3D_FEATURE_LEVEL featureLevel;
	HR(D3D11CreateDevice(
		vAdapters[useIdx],					// Default adapter
		md3dDriverType,
		0,									// No software device
		createDeviceFlags,
		0, 0,								// Default feature level array
		D3D11_SDK_VERSION,
		&mpd3dDevice,
		&featureLevel,
		&mpd3dImmediateContext));

	if (featureLevel != desiredFeatureLevel)
	{
		DBOUT("Direct3D Feature Level unsupported.");
		assert(false);
	}
    return false;
}
