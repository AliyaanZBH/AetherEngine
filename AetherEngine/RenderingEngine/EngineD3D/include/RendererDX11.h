#pragma once
//===============================================================================
// desc: The DX11 engine, handles everything backend related.
//		 Based upon one I built during a university project, but the goal is to optimize and modernize things going forward.
// auth: Aliyaan Zulfiqar
//===============================================================================
#ifndef D3DH
#define D3DH
//===============================================================================
#include <d3d11.h>
#include <wrl/client.h>
#include <cassert>

#include "IRenderer.h"
#include "AetherUtils.h"
//===============================================================================

class RendererDX11 : public IRenderer
{
public:
	// Main start up function
	bool Initialize(IWindow& window) override;
	void Render() override;
	void Terminate() override;

	// Is the screen/window square or letterbox or?
	float GetAspectRatio();

	// All rendering takes place inbetween these 2 function calls
	//void BeginRender(const DirectX::SimpleMath::Vector4& colour);
	void EndRender();

	// Default minimum behaviour when ALT+ENTER or drag or resize
	// Parameters are new width and height of window
	void OnResize_Default(int clientWidth, int clientHeight);

	// See mpOnResize
	void OnResize(int sw, int sh, RendererDX11& d3d)
	{
		/*assert(m_pOnResize);
		if (m_pOnResize)
			m_pOnResize(sw, sh, d3d);
		else*/
			OnResize_Default(sw, sh);;
	}

	void SetOnResize(void(*pOnResize)(int, int, RendererDX11&))
	{
		m_pOnResize = pOnResize;
	}

	void InitInputAssembler(ID3D11InputLayout* pInputLayout, ID3D11Buffer* pVBuffer, UINT szVertex, ID3D11Buffer* pIBuffer,
		D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// Public accessors
	//

	// Get a handle to the GPU
	const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice()				{ assert(m_pD3DDevice);	return m_pD3DDevice; }

	// Get a handle to be able to send commands to the GPU
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceCtx()	{ assert(m_pD3DImmediateContext); return m_pD3DImmediateContext; }

	// Get the GPU ready to render
	bool GetDeviceReady() const											{ return m_pD3DDevice != nullptr; }

	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetWrapSampler()								{ assert(m_pWrapSampler); return m_pWrapSampler; }

private:

	//
	// Heavy lifting to start D3D11
	//

	//	A device is used to create resources, this essentially represents our GPU
	bool CreateDevice();

	// Specify the type of swapchain needed
	void CreateSwapChainDescription(DXGI_SWAP_CHAIN_DESC& sd, HWND hMainWnd, bool windowed, int screenWidth, int screenHeight);

	// Once you have a description you can create the swapchain needed
	bool CreateSwapChain(DXGI_SWAP_CHAIN_DESC& sd);


	// Create all render buffers
	bool CreateRenderTargets();
	void CreateDepthStencilBufferAndView(D3D11_TEXTURE2D_DESC& dsd);

	// Bind the render target view and depth/stencil view to the pipeline.
	void BindRenderTargetViewAndDepthStencilView();

	// Buffers in the swap chain must match the screen resolution
	void ResizeSwapChain(int screenWidth, int screenHeight);

	// The kind of depth stencil we want
	void CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& dsd, int screenWidth, int screenHeight, bool msaa, int count, int maxQuality);

	// Viewport dimensions need updating when the window changes size
	void SetViewportDimensions(int screenWidth, int screenHeight);

	// Can this gpu support 4x sampling
	void CheckMultiSamplingSupport(UINT& quality4xMsaa);

	// Create the wrap sampler 
	void CreateWrapSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState>& pSampler);


	// Private members to facillitate the above functions
		// Main handle used to create resources and access D3D
	Microsoft::WRL::ComPtr<ID3D11Device> m_pD3DDevice = nullptr;

	// A handle of the device we can use to give rendering commands
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pD3DImmediateContext = nullptr;

	// A number of surfaces we can render onto and then display
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;

	// When we render things, where do they go?
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;

	// What sampler state do we want to use?
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pWrapSampler = nullptr;

	// What type of gpu have we got - hopefully a hardware one
	D3D_DRIVER_TYPE m_D3DDriverType = D3D_DRIVER_TYPE_UNKNOWN;

	// Depth buffer for sorting pixels by distance from camera
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer = nullptr;


	// When rendering we can test the depth of pixels, usually so we
	//	avoid rendering anything that is behind something else
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;

	//
	// Currently unused values that will be implemented soon
	//

	// Position, height, width, min+max depth of the view we are rendering
	D3D11_VIEWPORT m_ScreenViewport;

	// A function to call when we ALT+ENTER or drag the window
	// Two parameters are width/height of the new window and this
	void(*m_pOnResize)(int, int, RendererDX11&) = nullptr;


	// Running in a window?
	bool m_Windowed = false;
	// Multisampling anti-aliasing quality level supported
	UINT m_MsaaQuality = 0;
	bool m_Enable4xMsaa = true;
};

#endif