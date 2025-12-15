#pragma once
//===============================================================================
// desc: Dedicated ImGui layer for DX11
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiBaseLayer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ImGuiLayerDX11 : public ImGuiBaseLayer
	{
	public:
		ImGuiLayerDX11(eRenderAPI backend);
		~ImGuiLayerDX11();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	};
};