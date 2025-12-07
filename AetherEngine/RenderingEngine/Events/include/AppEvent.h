#pragma once
//===============================================================================
// desc: A set of application events that will fire and need handling
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Event.h"

namespace Aether
{
	class AETHER_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint16_t width, uint16_t height)
			:	m_Width(width), m_Height(height) {}

		inline uint16_t GetWidth() const { return m_Width; }
		inline uint16_t GetHeight() const { return m_Height; }

#if defined(DEBUG) || defined(_DEBUG)  

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}
#endif
		// Don't forget to implement the necessary static types, pass them in raw as the cast to EventType happens inside the macro and we also retain the utility to print the name as a string
		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(Application)
private:
	uint16_t m_Width, m_Height;
	};

	class AETHER_API WindowCloseEvent : public Event
	{
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(Application)
	};

	class AETHER_API AppTickEvent : public Event
	{
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(Application)
	};

	class AETHER_API AppUpdateEvent : public Event
	{
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(Application)
	};

	class AETHER_API AppRenderEvent : public Event
	{
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(Application)
	};
}