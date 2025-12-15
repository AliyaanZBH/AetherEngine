#pragma once
//===============================================================================
// desc: Dedicated ImGui layer for DX12
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiBaseLayer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ImGuiLayerDX12 : public ImGuiBaseLayer
	{
	public:
		ImGuiLayerDX12(eRenderAPI backend);
		~ImGuiLayerDX12();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	};
};