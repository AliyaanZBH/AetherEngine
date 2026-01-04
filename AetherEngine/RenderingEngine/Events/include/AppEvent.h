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
		EVENT_CLASS_TYPE(kWindowResize)
		EVENT_CLASS_CATEGORY(kApplicationEvent)
private:
	uint16_t m_Width, m_Height;
	};

	class AETHER_API WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent(bool focused)
			: m_bFocused(focused) {}

		inline const bool IsFocused() const { return m_bFocused; }

		EVENT_CLASS_TYPE(kWindowFocus)
		EVENT_CLASS_CATEGORY(kApplicationEvent)
	private:
		bool m_bFocused;
	};


	class AETHER_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(kWindowClose)
		EVENT_CLASS_CATEGORY(kApplicationEvent)
	};

	class AETHER_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(kAppTick)
		EVENT_CLASS_CATEGORY(kApplicationEvent)
	};

	class AETHER_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(kAppUpdate)
		EVENT_CLASS_CATEGORY(kApplicationEvent)
	};

	class AETHER_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(kAppRender)
		EVENT_CLASS_CATEGORY(kApplicationEvent)
	};
}