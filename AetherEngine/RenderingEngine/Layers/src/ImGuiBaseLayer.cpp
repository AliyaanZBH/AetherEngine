//===============================================================================
// desc: ImGui layer for clean separation within the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiBaseLayer.h"
#include "CoreApp.h"
//===============================================================================

namespace Aether
{
	ImGuiBaseLayer::ImGuiBaseLayer(eRenderAPI backend)
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

		ImGui::StyleColorsDark();
	}

	ImGuiBaseLayer::~ImGuiBaseLayer()
	{
	}

	void ImGuiBaseLayer::OnAttach()
	{
		// Create appropriate backends
		switch (m_CurrentRenderAPI)
		{
		case eRenderAPI::kOpenGL:
		{
			ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindowHandle()), true);
			ImGui_ImplOpenGL3_Init("#version 410");
			break;
		}
		case eRenderAPI::kDX11:
		{
			ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindowHandle()), true);    // Other for DX
			ImGui_ImplDX11_Init(static_cast<ID3D11Device*>(Application::Get().GetRenderer().GetNativeDevice()), static_cast<ID3D11DeviceContext*>(Application::Get().GetRenderer().GetNativeContext()));
			break;
		}
		case eRenderAPI::kDX12:
		{
			//ImGui_ImplDX12_Init();
			break;
		}
		default:
		{
			AETHER_ASSERT(AETHER_FAIL, "No rendering backend for ImGui defined.")
		}
		};

	}

	void ImGuiBaseLayer::OnDetach()
	{
	}

	void ImGuiBaseLayer::OnUpdate()
	{
		// Setup generic drawing frame, let derived classes implement specifics after this
		//

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight());
		io.DeltaTime = (float)glfwGetTime();

	}

	void ImGuiBaseLayer::OnEvent(Event& event)
    {
    }
}