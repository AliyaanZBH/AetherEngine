#pragma once
//===============================================================================
// desc: This file handles mouse events such as Moving and Clicking
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <sstream>	// TODO: Move to PCH

#include "Event.h"
//===============================================================================

namespace Aether
{
	class AETHER_API MouseEvent : public Event
	{
	public:
		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		// Setup category for all events that inherit from this
		EVENT_CLASS_CATEGORY(Mouse | Input)
	protected:
		// Protected constructor so that only it and derived class can construct it.
		// More of an abstract class
		MouseEvent(float currentX, float currentY) : m_MouseX(currentX), m_MouseY(currentY) {}
		// Represents a key on the keyboard
		float m_MouseX, m_MouseY;
	};
}