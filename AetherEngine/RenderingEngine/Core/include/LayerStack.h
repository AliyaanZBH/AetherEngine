#pragma once
//===============================================================================
// desc: Nice container with helper functions to manage engine layers
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Layer.h"
//===============================================================================
namespace Aether
{
	class AETHER_API LayerStack
	{
	public:
		LayerStack() { m_Layers.reserve(5); }
		~LayerStack();

		void HandleEvent(Event& e);

		void UpdateLayers();
		void RenderLayers();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

	private:
		std::vector<Layer*> m_Layers;

		// This is needed as we are storing layers and overlays in the same list, but we want to be able to push new layers to the end of the portion of the list containing the main layers,
		//		while mainting the position of all overlays at the very end of the list.
		uint8_t m_LayerInsertIndex = 0u;

	};
};