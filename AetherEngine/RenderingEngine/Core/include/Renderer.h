#pragma once
//===============================================================================
// desc: High-level API for applications using Aether to render games with ease
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "glm/glm.hpp"

#include "DrawCommand.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "Drawing.h"
//===============================================================================

namespace Aether
{
	class IRendererBackend;
	class IWindow;

	class AETHER_API Renderer
	{
	public:
		static void Initialise();
		static void Terminate();
		static void Resize(const uint16_t width, const uint16_t height);

		static void BeginFrame();
		static void EndFrame();

		static void DrawLine();
		static void DrawTriangle(const Transform& transform, const glm::vec4& colour);
		static void DrawQuad(const Transform& transform, const glm::vec4& colour);
		static void DrawCircle();
		static void DrawMesh();

		// TMP: I feel these are a breal the asbtraction a bit, but I'm putting them here just so we can get back to a stable build fast
		static void InitImGui();
		static void BeginImGuiRender();
		static void EndImGuiRender();


	private:

		//
		//	Set of lower-level functions that are hidden from the app to maintain the high-level abstraction, but must be implemented to facilitate the above.
		//

		static void CreateTriangleGeometry();
		static void CreateQuadGeometry();

		// Create a default high-level description for a rendering pipeline, built by the backend
		static void CreateBackendPipeline();

		// Signal to backend API to execute the command queue
		static void Dispatch();

		// Clears command queue ready for the next frame
		static void Flush();

		// Currently running renderer backend
		static std::unique_ptr<IRendererBackend> s_RendererBackend;

		// Per-frame render list
		static std::vector<DrawCommand> s_CommandQueue;

		//
		// Geometry containers - shared between renderer backends
		//

		static Buffer* s_PerDrawBuffer;
		static ConstantBufferView s_CBView;

		// Tri
		//

		static Buffer* s_TriVertexBuffer;
		static VertexBufferView* s_TriVBView;
		static Buffer* s_TriIndexBuffer;
		static IndexBufferView* s_TriIBView;


		// Quad
		//

		static Buffer* s_QuadVertexBuffer;
		static VertexBufferView* s_QuadVBView;
		static Buffer* s_QuadIndexBuffer;
		static IndexBufferView* s_QuadIBView;
	};
}