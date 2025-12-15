//===============================================================================
// desc: Dedicated ImGui layer for DX11
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiLayerDX11.h"
//===============================================================================

namespace Aether
{
	ImGuiLayerDX11::ImGuiLayerDX11(eRenderAPI backend)
		: ImGuiBaseLayer(backend)
	{
	}

	ImGuiLayerDX11::~ImGuiLayerDX11()
	{
	}

	void ImGuiLayerDX11::OnAttach()
	{
		// Create appropriate backend
		ImGuiBaseLayer::OnAttach();
	}

	void ImGuiLayerDX11::OnDetach()
	{
	}

	void ImGuiLayerDX11::OnUpdate()
	{
		// Call base update which sets up the generic ImGui Frame for us
		ImGuiBaseLayer::OnUpdate();

		// Now proceed with specific implementation and update path

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}

	void ImGuiLayerDX11::OnEvent(Event& event)
    {
    }
}