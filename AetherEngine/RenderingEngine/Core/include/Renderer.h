#pragma once
//===============================================================================
// desc: High-level API for applications using Aether to render games with ease
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "glm/glm.hpp"

#include "Pipeline.h"
#include "Buffer.h"
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

		static void BeginFrame();
		static void EndFrame();

		static void DrawLine();
		static void DrawTriangle();
		static void DrawQuad();
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


		// Instead of submitting draw functions immediately, submit them to a queue so that they can all be drawn safely at the correct time for each rendering API.
		// This abstraction helps with app users not having to worry about when they call a draw function
		struct DrawCommand
		{
			enum class eDrawType { kLine, kTri, kQuad, kCircle, kMesh} m_Type;
			glm::mat4 m_Transform;
			glm::vec4 m_Colour;
			//const Mesh* m_Mesh = nullptr;
		};

		// Per-frame render list
		static std::vector<DrawCommand> s_CommandQueue;

		//
		// Geometry containers - shared between renderer backends
		//

		// Quads
		//

		static Buffer* s_QuadVertexBuffer;
		static VertexBufferView s_QuadVBView;
		static Buffer* s_QuadIndexBuffer;
		static IndexBufferView s_QuadIBView;
	};
}