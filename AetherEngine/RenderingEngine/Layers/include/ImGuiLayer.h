#pragma once
//===============================================================================
// desc: ImGui layer for clean separation within the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Layer.h"
#include "IRenderer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(eRenderAPI backend);
		~ImGuiLayer();

		void UpdateRenderAPI(eRenderAPI newBackend) { m_CurrentRenderAPI = newBackend; }

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		eRenderAPI m_CurrentRenderAPI;
	};
};