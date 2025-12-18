#pragma once
//===============================================================================
// desc: This file handles mouse events such as Moving and Clicking
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Event.h"
//===============================================================================

namespace Aether
{
	class AETHER_API MouseEvent : public Event
	{
	public:
		// Register category this event belongs to
		EVENT_CLASS_CATEGORY(kMouse | kInput)
	protected:
		// Protected constructor so that only it and derived class can construct it.
		// More of an abstract class
		MouseEvent() {}
	};

	class AETHER_API MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y), MouseEvent() {}		// Remember to call parent constructor so that the class gets the right category!

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMoveEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(kMouseMove)
	private:
		// Represents a position on the window
		float m_MouseX, m_MouseY;
	};

	class AETHER_API MouseScrollEvent : public MouseEvent
	{
	public:
		MouseScrollEvent(float x, float y)
			: m_ScrollX(x), m_ScrollY(y), MouseEvent() {}

		inline float GetScrollX() const { return m_ScrollX; }
		inline float GetScrollY() const { return m_ScrollY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent: " << m_ScrollX << ", " << m_ScrollY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(kMouseScroll)
	private:
		float m_ScrollX, m_ScrollY;
	};

	// Another abstract-y layer for button events
	class AETHER_API MouseButtonEvent : public MouseEvent
	{
	public:
		inline int GetMouseButton() const { return m_Button; }
	protected:
		MouseButtonEvent(const int button)
			: m_Button(button), MouseEvent() {}
		int m_Button;
	};

	class AETHER_API MouseClickEvent : public MouseButtonEvent
	{
	public:
		MouseClickEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseClickEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(kMouseClick)
	};

	class AETHER_API MouseClickReleaseEvent : public MouseButtonEvent
	{
	public:
		MouseClickReleaseEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseClickReleaseEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(kMouseClickRelease)
	};
}