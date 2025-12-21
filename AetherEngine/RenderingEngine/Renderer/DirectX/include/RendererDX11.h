#pragma once
//===============================================================================
// desc: The DX11 engine, handles everything backend related.
//		 Based upon one I built during a university project, but the goal is to optimize and modernize things going forward.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IRenderer.h"
//===============================================================================

namespace Aether
{
	class RendererDX11 final : public IRenderer
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


		void* GetNativeDevice() override { return m_pD3DDevice.Get(); }
		void* GetNativeContext() override { return m_pD3DImmediateContext.Get(); }

		void InitImGui() override;
		void BeginImGuiRender() override;
		void EndImGuiRender() override;

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

		//
		// Public accessors
		//

		// Get a handle to the GPU
		const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() { assert(m_pD3DDevice);	return m_pD3DDevice; }

		// Get a handle to be able to send commands to the GPU
		const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceCtx() { assert(m_pD3DImmediateContext); return m_pD3DImmediateContext; }

		// Get the GPU ready to render
		bool GetDeviceReady() const { return m_pD3DDevice != nullptr; }

		const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetWrapSampler() { assert(m_pWrapSampler); return m_pWrapSampler; }

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

		// The kind of depth stencil we want
		void CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& dsd, int screenWidth, int screenHeight, bool msaa, int count, int maxQuality);;

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

		// Running in a window?
		bool m_Windowed = false;
		// Multisampling anti-aliasing quality level supported
		UINT m_MsaaQuality = 0;
		bool m_Enable4xMsaa = true;
	};
};