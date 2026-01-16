#pragma once
//===============================================================================
// desc: Interface for an abstract rendering API, to be implemented by DirectX, OpenGL and Vulkan.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IWindow.h"
#include "AetherUtils.h"
//===============================================================================

namespace Aether
{
	class Buffer;
	struct BufferDesc;
	struct PipelineDesc;
	struct VertexBufferView;
	struct IndexBufferView;
	struct ConstantBufferView;
	struct MaterialBufferView;
	struct DrawCommand;

	using PipelineHandle = uint32_t;

	class AETHER_API IRendererBackend {
	public:
		virtual ~IRendererBackend() = default;

		virtual AETHER_RESULT Initialize(const IWindow& window) = 0;
		virtual void CreatePipeline(const PipelineDesc& desc, const PipelineHandle handle) = 0;
		virtual void BindPipeline(const PipelineHandle handle) = 0;
		virtual void BindGlobalResources(Buffer* materialBuffer) = 0;

		virtual void ClearFrame() = 0;
		virtual void Render() = 0;
		// Take a high-level draw call and break it down into an optimised render call for each API.
		virtual void Submit(const DrawCommand& cmd, ConstantBufferView* cbv) = 0;
		virtual void Render(VertexBufferView* vbv, IndexBufferView* ibv) {};
		virtual void Present() = 0;

		virtual void Resize(int newWidth, int newHeight) = 0;
		virtual void Terminate() = 0;

		virtual Buffer* CreateBuffer(const BufferDesc& desc) = 0;
		virtual void BindFrameConstants(const ConstantBufferView* cbv) = 0;
		virtual void FinalizeUploads() = 0; 

		virtual void* GetNativeDevice() = 0;
		virtual void* GetNativeContext() = 0;

		// To be called by ImGui layer
		virtual void InitImGui() = 0;
		virtual void BeginImGuiRender() = 0;
		virtual void EndImGuiRender() = 0;
	};
}