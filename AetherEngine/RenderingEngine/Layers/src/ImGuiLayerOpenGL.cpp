//===============================================================================
// desc: Dedicated ImGui layer for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiLayerOpenGL.h"
//===============================================================================

namespace Aether
{
	ImGuiLayerOpenGL::ImGuiLayerOpenGL(eRenderAPI backend)
		: ImGuiBaseLayer(backend)
	{
	}

	ImGuiLayerOpenGL::~ImGuiLayerOpenGL()
	{
	}

	void ImGuiLayerOpenGL::OnAttach()
	{
		// Create appropriate backend
		ImGuiBaseLayer::OnAttach();
	}

	void ImGuiLayerOpenGL::OnDetach()
	{
	}

	void ImGuiLayerOpenGL::OnUpdate()
	{
		// Call base update which sets up the generic ImGui Frame for us
		ImGuiBaseLayer::OnUpdate();

		// Now proceed with specific implementation and update path
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}

	void ImGuiLayerOpenGL::OnEvent(Event& event)
    {
    }
}