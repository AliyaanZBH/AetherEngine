#pragma once
//===============================================================================
// desc: This file handles Keyboard events such as Pressed and Released
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <sstream>	// TODO: Move to PCH

#include "Event.h"
//===============================================================================

namespace Aether
{
	class AETHER_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode()	  const  { return m_KeyCode; }

		// Setup category for all events that inherit from this
		EVENT_CLASS_CATEGORY(Keyboard | Input)
	protected:
		// Protected constructor so that only it and derived class can construct it.
		// More of an abstract class
		KeyEvent(int keycode) : m_KeyCode(keycode) {}
		// Represents a key on the keyboard
		int m_KeyCode;
	};

	// This class will actually be called from the Engine
	class AETHER_API KeyPressedEvent : public KeyEvent
	{
	public:
		// Takes in a keycode and also whether the input is being held and repeated
		KeyPressedEvent(int keycode, bool repeat) : KeyEvent(keycode), m_bRepeat(repeat) {}

		bool IsRepeat() const { return m_bRepeat; }

		// Override from Event base class to give more detailed debugging specific to key events
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeating = " << m_bRepeat << ")";
			return ss.str();
		}

		// Define the type within the event system
		EVENT_CLASS_TYPE(KeyPressed)
	private:
		// Flag that tracks if a given input is being held
		bool m_bRepeat;
	};

	class AETHER_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	// Used for typing - e.g. allowing players or devs to name an object in their application
	class AETHER_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}