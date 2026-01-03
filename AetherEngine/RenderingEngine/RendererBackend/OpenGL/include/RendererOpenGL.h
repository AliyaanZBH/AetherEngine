#pragma once
//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IRendererBackend.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
//===============================================================================
namespace Aether
{
	class RendererOpenGL final : public IRendererBackend
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

		void Resize(int newWidth, int newHeight) override {}	// Empty for now since GLFW seemingly handles this for us!
		void Terminate() override;

		Buffer* CreateBuffer(const BufferDesc& desc) override;
		void BindFrameConstants(const ConstantBufferView* cbv) override;

		void* GetNativeDevice() override { return 0; }
		void* GetNativeContext() override { return 0; }

		void InitImGui() override;
		void BeginImGuiRender() override;
		void EndImGuiRender() override;
	private:
		ShaderOpenGL* LoadShader(ShaderHandle vertHandle, ShaderHandle fragHandle);
		
		void BindConstantBuffer(const ConstantBufferView* cbv);

		GLFWwindow* m_pWindow;

		unsigned int m_VertexAttributeArray = 0;
		int m_DynamicColourLocation = 0;
		int m_TransformLocation = 0;
		int m_SolidColourLocation = 0;

		// In OpenGL, multiple shaders must be linked into a single shader program, this one contains a vertex and pixel shader
		ShaderOpenGL* m_DefaultShader = nullptr;
		std::unordered_map<ShaderHandle, ShaderOpenGL*> m_ShaderCache;

		BufferOpenGL* m_VertexBuffer = nullptr;
		BufferOpenGL* m_IndexBuffer = nullptr;

	};
};