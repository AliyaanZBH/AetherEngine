#pragma once
//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IRendererBackend.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
#include "PipelineOpenGL.h"
//===============================================================================
namespace Aether
{
	class RendererOpenGL final : public IRendererBackend
	{
	public:
		// Main start up function
		AETHER_RESULT Initialize(const IWindow& window) override;
		void CreatePipeline(const PipelineDesc& desc, const PipelineHandle handle) override;
		void BindPipeline(const PipelineHandle handle) override;
		void BindGlobalResources(Buffer* materialBuffer) override;

		void ClearFrame() override;
		void Render() override;
		void Submit(const DrawCommand& cmd, ConstantBufferView* cbv) override;
		void Render(VertexBufferView* vbv, IndexBufferView* ibv) override;
		void Present() override;

		void Resize(int newWidth, int newHeight) override {}	// Empty for now since GLFW seemingly handles this for us!
		void Terminate() override;

		Buffer* CreateBuffer(const BufferDesc& desc) override;
		void BindFrameConstants(const ConstantBufferView* cbv) override;
		void FinalizeUploads() override;

		void* GetNativeDevice() override { return 0; }
		void* GetNativeContext() override { return 0; }

		void InitImGui() override;
		void BeginImGuiRender() override;
		void EndImGuiRender() override;
	private:
		ShaderOpenGL* LoadShader(const ShaderHandle vertHandle, const ShaderHandle fragHandle);
		void BindConstantBuffer(const ConstantBufferView* cbv);

		// Gets or lazily creates VAOs for a unique pair of geometry + pipeline
		GLuint LoadVAO(const GLuint vb, const GLuint ib);

		GLFWwindow* m_pWindow;

		// Cache current pipeline handle to use to look up our VAO cache
		PipelineHandle m_CurrentPipeline;

		// Cache of pipelines that we can bind for materials, sets shaders too
		std::unordered_map<PipelineHandle, PipelineOpenGL*> m_PipelineCache;
		
		//std::unordered_map<VAOKey, GLuint, VAOKeyHasher> m_VAOCache;
		std::unordered_map<PipelineHandle, GLuint> m_VAOCache;

		BufferOpenGL* m_VertexBuffer = nullptr;
		BufferOpenGL* m_IndexBuffer = nullptr;

	};
};
