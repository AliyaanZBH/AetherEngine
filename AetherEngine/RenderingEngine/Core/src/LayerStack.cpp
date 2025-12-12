//===============================================================================
// desc: Nice container with helper functions to manage engine layers
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "LayerStack.h"
//===============================================================================

namespace Aether
{
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::HandleEvent(Event& e)
	{
		// Go down the layer stack, ensuring top-level layers (e.g overlays) have their events fired and handled first, before lower-level ones.
		for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
		{
			// Check if the event was already handled (this gets set when OnEvent fires and successfully completes) and break out of the loop if it has
			if (e.CheckIfHandled())
				break;
			(*it)->OnEvent(e); // Event has not been handled yet, try completing in this layer
		}
	}

	void LayerStack::UpdateLayers()
	{
		for (Layer* layer : m_Layers)
			layer->OnUpdate();
	}

	// Push layer to end of the layer section of our list, just in front of the first overlay.
	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		// Update the index so that we now where to put the next layer
		m_LayerInsertIndex++;
	}

	// Overlays are simple, they won't ever get inserted in the middle
	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (it != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}


}