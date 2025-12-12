#pragma once
//===============================================================================
// desc: Base class for Engine layers
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "Event.h"
//===============================================================================

namespace Aether
{
	class AETHER_API Layer
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_DebugName(name) {}

		virtual ~Layer() = default;
		
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}