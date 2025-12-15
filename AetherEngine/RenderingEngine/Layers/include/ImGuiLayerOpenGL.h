#pragma once
//===============================================================================
// desc: Dedicated ImGui layer for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ImGuiBaseLayer.h"
//===============================================================================

namespace Aether
{
	class AETHER_API ImGuiLayerOpenGL : public ImGuiBaseLayer
	{
	public:
		ImGuiLayerOpenGL(eRenderAPI backend);
		~ImGuiLayerOpenGL();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	};
};