//===============================================================================
// desc: ImGui layer for clean separation within the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiLayer.h"
#include <CoreApp.h>
//===============================================================================

namespace Aether
{
    ImGuiLayer::ImGuiLayer(eRenderAPI backend)
        : m_CurrentRenderAPI(backend), Layer("ImGui Layer")
    {
    }

    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        // Setup ImGui Context
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
       // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
      //  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        ImGui::StyleColorsDark();

        gladLoadGL();
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindowHandle()), true);
        ImGui_ImplOpenGL3_Init("#version 410");

        // Create appropriate backends
       //switch (m_CurrentRenderAPI)
       //{
       //case eRenderAPI::kOpenGL:
       //{
       //    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindowHandle()), true);
       //    ImGui_ImplOpenGL3_Init("#version 410");
       //    break;
       //}
       //case eRenderAPI::kDX11:
       //{
       //    //ImGui_ImplGlfw_InitForOther
       //    //ImGui_ImplDX11_Init();
       //    break;
       //}
       //case eRenderAPI::kDX12:
       //{
       //    //ImGui_ImplDX12_Init();
       //    break;
       //}
       //default:
       //{
       //    AETHER_ASSERT(AETHER_FAIL, "No rendering backend for ImGui defined.")
       //}
       //};

    }

    void ImGuiLayer::OnDetach()
    {
    }

    void ImGuiLayer::OnUpdate()
    {

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight());
        io.DeltaTime = (float)glfwGetTime();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        bool show = true;
        ImGui::ShowDemoWindow(&show);

        glClearColor(0.2f, 0.7f, 0.9f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    }

    void ImGuiLayer::OnEvent(Event& event)
    {
    }
}