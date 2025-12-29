#pragma once
//===============================================================================
// desc: High-level API for applications using Aether to render games with ease
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "glm/glm.hpp"
//===============================================================================

namespace Aether
{
	enum class eRenderAPI;
	class IRendererBackend;
	class IWindow;

	class AETHER_API Renderer
	{
	public:
		static void Initialise(IWindow& window);
		static void Terminate();

		static void Begin();
		static void End();

		static void DrawLine();
		static void DrawTriangle();
		static void DrawQuad();
		static void DrawCircle();
		static void DrawMesh();

	private:
		// Signal to backend API to execute the command queue
		static void Dispatch();

		// Clears command queue ready for the next frame
		static void Flush();

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

		static std::unique_ptr<IRendererBackend> s_RendererBackend;

	};
}