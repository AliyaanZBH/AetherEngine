//===============================================================================
// desc: ImGui layer for clean separation within the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiLayer.h"
//===============================================================================

Aether::ImGuiLayer::ImGuiLayer()
	: Layer("ImGui Layer")
{

}

Aether::ImGuiLayer::~ImGuiLayer()
{
}

void Aether::ImGuiLayer::OnAttach()
{
	// Setup ImGui Context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	ImGui::StyleColorsDark();

}

void Aether::ImGuiLayer::OnDetach()
{
}

void Aether::ImGuiLayer::OnUpdate()
{
}

void Aether::ImGuiLayer::OnEvent(Event& event)
{
}
