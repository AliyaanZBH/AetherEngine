#pragma once
//===============================================================================
// desc: The DX11 engine, handles everything backend related.
//		 Based upon one I built during a university project, but the goal is to optimize and modernize things going forward.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IRendererBackend.h"
#include "ShaderDX11.h"
#include "BufferDX11.h"
#include "Vertex.h"
//===============================================================================

namespace Aether
{
	class RendererDX11 final : public IRendererBackend
	{
	public:
		// Main start up function
		AETHER_RESULT Initialize(const IWindow& window) override;
		void CreatePipeline(const PipelineDesc& desc) override;

		void ClearFrame() override;
		void Render() override;
		void Submit(const DrawCommand& cmd, ConstantBufferView* cbv) override;
		void Render(VertexBufferView* vbv, IndexBufferView* ibv) override;
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
			OnResize_Default(sw, sh);
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

		AETHER_RESULT CreateDevice();
		AETHER_RESULT CreateSwapChain(DXGI_SWAP_CHAIN_DESC& sd);
		AETHER_RESULT CreateRenderTargets();
		AETHER_RESULT CreateRasteriserState();
		AETHER_RESULT CreateDepthStencilBufferAndView(D3D11_TEXTURE2D_DESC& dsd);

		void CreateSwapChainDescription(DXGI_SWAP_CHAIN_DESC& sd, HWND hMainWnd, bool windowed, int screenWidth, int screenHeight);
		void CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& dsd, int screenWidth, int screenHeight, bool msaa, int count, int maxQuality);;
		void CreateWrapSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState>& pSampler);

		void UpdateViewportAndScissor();

		//
		// Private utility functions
		// 
		
		// Either finds an already compiled shader in the cache or compiles and inserts a new one
		ShaderDX11* LoadShader(ShaderHandle handle);

		// Translate API agnostic input layout into DX11 land
		std::vector<D3D11_INPUT_ELEMENT_DESC> TranslateLayout(const VertexLayout& layout);

		//
		// Private members to facillitate the above functions
		//


		Microsoft::WRL::ComPtr<ID3D11Device> m_pD3DDevice = nullptr;

		// A handle of the device we can use to give rendering commands
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pD3DImmediateContext = nullptr;

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterState = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pWrapSampler = nullptr;
		D3D_DRIVER_TYPE m_D3DDriverType = D3D_DRIVER_TYPE_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;


		// Use our shader wrapper to handle shader files themselves
		std::unordered_map<ShaderHandle, ShaderDX11*> m_ShaderCache;

		ID3D11VertexShader* m_VS = nullptr;
		ID3D11PixelShader* m_PS = nullptr;
		ID3D11InputLayout* m_Layout = nullptr;
		
		//
		//	Window data!
		//
		WindowContext::WinData m_WinData = {};

		// Position, height, width, min+max depth of the view we are rendering
		D3D11_VIEWPORT m_Viewport;
		D3D11_RECT m_Scissor;

		//
		// Currently unused values that will be implemented soon
		//


		// Running in a window?
		bool m_Windowed = false;
		// Multisampling anti-aliasing quality level supported
		UINT m_MsaaQuality = 0;
		bool m_Enable4xMsaa = true;
	};
};