//===============================================================================
// desc: ImGui layer for clean separation within the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiLayer.h"
#include "CoreApp.h"
//===============================================================================

namespace Aether
{
	ImGuiLayer::ImGuiLayer(eRenderAPI backend)
		: m_CurrentRenderAPI(backend), Layer("ImGui Layer")
	{
		// Setup ImGui Context
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;			// Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;			// Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;				// Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Set up our custom style!
		SetStyle();
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		// Create appropriate backend
		switch (m_CurrentRenderAPI)
		{
		case eRenderAPI::kOpenGL:
		{
			ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindowHandle()), true);
			break;
		}
		case eRenderAPI::kDX11:
		case eRenderAPI::kDX12:
		{
			ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindowHandle()), true);    // Other is needed for DX
			break;
		}
		default:
		{
			AETHER_ASSERT(AETHER_FAIL, "No rendering backend for ImGui defined.")
		}
		};

		// Bit more work is needed to get DX12 up and running with ImGui, so created a virtual function in the interface that accounts for all of them now! 
		Application::Get().GetRenderer().InitImGui();
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		// Setup generic drawing frame, let derived classes implement specifics after this
		//

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight());
		io.DeltaTime = (float)glfwGetTime();


		// Now proceed with specific rendering API path
		Application::Get().GetRenderer().RenderImGui();

	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}

	void ImGuiLayer::SetStyle()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_Text] = GUIThemeColours::m_PureWhite;
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = GUIThemeColours::m_Charcoal;
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = GUIThemeColours::m_Charcoal;
		colors[ImGuiCol_Border] = GUIThemeColours::m_PureBlack;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = GUIThemeColours::m_GunmetalGrey;
		colors[ImGuiCol_FrameBgHovered] = GUIThemeColours::m_DarkestGold;
		colors[ImGuiCol_FrameBgActive] = GUIThemeColours::m_AetherGold;
		colors[ImGuiCol_TitleBg] = GUIThemeColours::m_DarkerGold;
		colors[ImGuiCol_TitleBgActive] = GUIThemeColours::m_DarkerGold;
		colors[ImGuiCol_TitleBgCollapsed] = GUIThemeColours::m_DarkestGold;
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.25f, 0.25f, 0.5f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.f, 0.171f, 0.f, 0.9f);
		colors[ImGuiCol_Button] = GUIThemeColours::m_AetherBlack;
		colors[ImGuiCol_ButtonHovered] = GUIThemeColours::m_DarkestGold;
		colors[ImGuiCol_ButtonActive] = GUIThemeColours::m_AetherGold;
		colors[ImGuiCol_Header] = GUIThemeColours::m_AetherLightBlack;
		colors[ImGuiCol_HeaderHovered] = GUIThemeColours::m_DarkestGold;
		colors[ImGuiCol_HeaderActive] = GUIThemeColours::m_AetherGold;
		colors[ImGuiCol_Separator] = GUIThemeColours::m_AetherGold;
		colors[ImGuiCol_SeparatorHovered] = ImVec4(1.f, 0.171f, 0.f, 1.f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(1.f, 0.1f, 0.f, 1.f);
		colors[ImGuiCol_ResizeGrip] = GUIThemeColours::m_AetherGold;
		colors[ImGuiCol_ResizeGripHovered] = GUIThemeColours::m_DarkerGold;
		colors[ImGuiCol_ResizeGripActive] = GUIThemeColours::m_DarkestGold;
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.f, 0.5f, 1.f, 1.f);                   // Gamepad / KBM highlight.
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}
}