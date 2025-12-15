//===============================================================================
// desc: Dedicated ImGui layer for DX12
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiLayerDX12.h"
//===============================================================================

namespace Aether
{
	ImGuiLayerDX12::ImGuiLayerDX12(eRenderAPI backend)
		: ImGuiBaseLayer(backend)
	{
	}

	ImGuiLayerDX12::~ImGuiLayerDX12()
	{
	}

	void ImGuiLayerDX12::OnAttach()
	{
		// Create appropriate backend
		ImGuiBaseLayer::OnAttach();
	}

	void ImGuiLayerDX12::OnDetach()
	{
	}

	void ImGuiLayerDX12::OnUpdate()
	{
		// Call base update which sets up the generic ImGui Frame for us
		ImGuiBaseLayer::OnUpdate();

		// Now proceed with specific implementation and update path

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData());

	}

	void ImGuiLayerDX12::OnEvent(Event& event)
    {
    }
}