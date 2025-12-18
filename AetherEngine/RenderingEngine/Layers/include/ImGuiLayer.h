#pragma once
//===============================================================================
// desc: ImGui base layer for clean separation within the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Layer.h"
#include "IRenderer.h"
//===============================================================================

namespace Aether
{
	// Pallete consts for ease of use, readabilty and editability
	namespace GUIThemeColours
	{
		// const ImVec4 m_HighlightColour = ImVec4(1.0f, 1.f, 0.4f, 1.f);
		const ImVec4 m_PureBlack = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		const ImVec4 m_PureWhite = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		const ImVec4 m_AetherGold = ImVec4(1.f, 0.8f, 0.f, 1.f);

		const ImVec4 m_AetherBlack = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		const ImVec4 m_AetherLightBlack = ImVec4(0.04f, 0.04f, 0.04f, 0.85f);
		const ImVec4 m_PurgatoryGrey = ImVec4(0.25f, 0.25f, 0.25f, 0.5f);
		const ImVec4 m_Charcoal = ImVec4(0.15f, 0.15f, 0.21f, 0.965f);
		const ImVec4 m_GunmetalGrey = ImVec4(0.068f, 0.068f, 0.068f, 0.965f);
		const ImVec4 m_DarkerGold = ImVec4(0.9f, 0.7f, 0.f, 0.9f);
		const ImVec4 m_DarkestGold = ImVec4(0.8f, 0.6f, 0.f, 0.8f);;
		const ImVec4 m_BasicallyRed = ImVec4(0.9f, 0.05f, 0.f, 0.9f);

		const ImVec4 m_HighlightColour = m_AetherGold;
	};

	class AETHER_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(eRenderAPI backend);
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(Event& event) override;

		void SetStyle();
	protected:
		eRenderAPI m_CurrentRenderAPI;
	};
};