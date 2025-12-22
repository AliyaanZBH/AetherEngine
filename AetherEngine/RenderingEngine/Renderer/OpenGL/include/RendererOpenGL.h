#pragma once
//===============================================================================
// desc: The OpenGL rendering engine, leveraging GLAD.
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "IRenderer.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
//===============================================================================
namespace Aether
{
	class RendererOpenGL final : public IRenderer
	{
	public:
		// Main start up function
		AETHER_RESULT Initialize(IWindow& window) override;
		void ClearFrame() override;
		void Render() override;
		void Render(VertexBufferView* vbv, IndexBufferView* ibv) override;
		void Present() override;
		void Resize(int newWidth, int newHeight) override {}	// Empty for now since GLFW seemingly handles this for us!
		void Terminate() override;

		Buffer* CreateBuffer(const BufferDesc& desc) override;

		void* GetNativeDevice() override { return 0; }
		void* GetNativeContext() override { return 0; }

		void InitImGui() override;
		void BeginImGuiRender() override;
		void EndImGuiRender() override;
	private:
		GLFWwindow* m_pWindow;

		unsigned int m_VertexAttributeArray;

		ShaderOpenGL* m_Shader = nullptr;
		BufferOpenGL* m_VertexBuffer = nullptr;
		BufferOpenGL* m_IndexBuffer = nullptr;
	};
};