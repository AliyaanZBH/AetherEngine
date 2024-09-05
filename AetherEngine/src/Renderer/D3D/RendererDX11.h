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

#include "IRenderer.h"
//===============================================================================
class RendererDX11 : public IRenderer
{
public:
	bool Initialize() override;
	void RenderFrame() override;
	void Shutdown() override;

	// Main start up function
	bool InitDirect3D(void(*pOnResize)(int, int, MyD3D&));

	// Default minimum behaviour when ALT+ENTER or drag or resize
	// Parameters are new width and height of window
	void OnResize_Default(int clientWidth, int clientHeight);

	// Main shutdown function, don't forget to call it
	// ExtraReporting gives a bit more information about any objects
	//	that we haven't released properly
	void ReleaseD3D(bool extraReporting = true);

	// Is the screen/window square or letterbox or?
	float GetAspectRatio();

	// All rendering takes place inbetween these 2 function calls
	void BeginRender(const DirectX::SimpleMath::Vector4& colour);
	void EndRender();

	// Get a handle to the GPU
	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice()
	{
		assert(mpd3dDevice);
		return mpd3dDevice;
	}

	// Get a handle to be able to send commands to the GPU
	ID3D11DeviceContext& GetDeviceCtx() {
		assert(mpd3dImmediateContext);
		return *mpd3dImmediateContext;
	}

	// Get the GPU ready to render
	bool GetDeviceReady() const
	{
		return mpd3dDevice != nullptr;
	}

	// See mpOnResize
	void OnResize(int sw, int sh, MyD3D& d3d)
	{
		assert(mpOnResize);
		if (mpOnResize)
			mpOnResize(sw, sh, d3d);
		else
			OnResize_Default(sw, sh);;
	}

	void SetOnResize(void(*pOnResize)(int, int, MyD3D&))
	{
		mpOnResize = pOnResize;
	}

	ID3D11SamplerState& GetWrapSampler()
	{
		assert(mpWrapSampler);
		return *mpWrapSampler;
	}
	void InitInputAssembler(ID3D11InputLayout* pInputLayout, ID3D11Buffer* pVBuffer, UINT szVertex, ID3D11Buffer* pIBuffer,
		D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

private:
	// Main handle used to create resources and access D3D
	Microsoft::WRL::ComPtr<ID3D11Device> mpd3dDevice = nullptr;

	// A handle of the device we can use to give rendering commands
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mpd3dImmediateContext = nullptr;

	// A number of surfaces we can render onto and then display
	Microsoft::WRL::ComPtr<IDXGISwapChain> mpSwapChain = nullptr;

	// When we render things, where do they go?
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mpRenderTargetView = nullptr;


	// What type of gpu have we got - hopefully a hardware one
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;

	// Multisampling anti-aliasing quality level supported
	UINT m4xMsaaQuality = 0;
	bool mEnable4xMsaa = true;

	// Depth buffer for sorting pixels by distance from camera
	ID3D11Texture2D* mpDepthStencilBuffer = nullptr;

	// Running in a window?
	bool mWindowed = false;


	//
	// Heavy lifting to start D3D11
	//

	
	//	A device is used to create resources, this essentially represents our GPU
	bool CreateDevice();

	// Specify the type of swapchain needed
	void CreateSwapChainDescription(DXGI_SWAP_CHAIN_DESC& sd, HWND hMainWnd, bool windowed, int screenWidth, int screenHeight);

	// Once you have a description you can create the swapchain needed
	void CreateSwapChain(DXGI_SWAP_CHAIN_DESC& sd);

	// When rendering we can test the depth of pixels, usually so we
	//	avoid rendering anything that is behind something else
	ID3D11DepthStencilView* mpDepthStencilView = nullptr;

	// Position, height, width, min+max depth of the view we are rendering
	D3D11_VIEWPORT mScreenViewport;

	// A function to call when we ALT+ENTER or drag the window
	// Two parameters are width/height of the new window and this
	void(*mpOnResize)(int, int, MyD3D&) = nullptr;

	// What sampler state do we want to use?
	ID3D11SamplerState* mpWrapSampler = nullptr;
	void CreateD3D(D3D_FEATURE_LEVEL desiredFeatureLevel = D3D_FEATURE_LEVEL_11_0);

	// Buffers in the swap chain must match the screen resolution
	void ResizeSwapChain(int screenWidth, int screenHeight);

	// The kind of depth stencil we want
	void CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& dsd, int screenWidth, int screenHeight, bool msaa4X, int maxQuality);

	// Create all render buffers
	void CreateDepthStencilBufferAndView(D3D11_TEXTURE2D_DESC& dsd);

	// Bind the render target view and depth/stencil view to the pipeline.
	void BindRenderTargetViewAndDepthStencilView();

	// Viewport dimensions need updating when the window changes size
	void SetViewportDimensions(int screenWidth, int screenHeight);

	// Can this gpu support 4x sampling
	void CheckMultiSamplingSupport(UINT& quality4xMsaa);

	// Create the wrap sampler 
	void CreateWrapSampler(ID3D11SamplerState*& pSampler);

};

#endif