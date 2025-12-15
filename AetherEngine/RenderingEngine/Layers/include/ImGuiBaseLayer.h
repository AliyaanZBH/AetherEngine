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
	class AETHER_API ImGuiBaseLayer : public Layer
	{
	public:
		ImGuiBaseLayer(eRenderAPI backend);
		virtual ~ImGuiBaseLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	protected:
		eRenderAPI m_CurrentRenderAPI;
	};
};