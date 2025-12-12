#pragma once
//===============================================================================
// desc: ImGui layer for clean separation within the engine
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Layer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
	};
};