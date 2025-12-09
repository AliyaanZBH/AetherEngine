#pragma once
//===============================================================================
// desc: The main file that ties the whole Event System together
// auth: Aliyaan Zulfiqar
//===============================================================================

#include "Core.h"
//===============================================================================

namespace Aether
{
	//
	//	Events are currently first come first served. An event queue will be implemented soon,
	//		so that we can buffer events and process them during the "Event" part of the update stage
	//

	// What events we have in our engine and our application
	enum class EventType
	{
		None = 0,
		KeyPressed, KeyReleased, KeyTyped,
		MouseMoved, MouseClicked, MouseClickReleased, MouseScrolled,
		WindowClose, WindowMoved, WindowResize, WindowFocus, WindowLostFocus,
		AppUpdate, AppRender, AppTick
	};

	// Helps with filtering events, for example when ordering our event queue
	// Using a bit field as events can fall under multiple categories
	enum EventCategory
	{
		None = 0,
		ApplicationEvent	= BIT(0),
		Input				= BIT(1),
		Mouse				= BIT(2),		// For movement and scrolling
		Click				= BIT(3),		// For clicking LMB etc.
		Keyboard			= BIT(4),
	};

	//
	// Macros to allow quick event creation and avoid duplicating and rewriting a whole buncha code 
	//

	// The static types are here so that we can read key events without being tied to a specific instance of the event class.
#define EVENT_CLASS_TYPE(type)				static EventType GetStaticType()				{ return EventType::##type; }	\
											virtual EventType GetEventType() const override { return GetStaticType(); }		\
											virtual const char* GetName()    const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)		virtual int GetCategories()		 const override { return category; }

	//
	// The actual event classes
	//

	// Base class / interface for every Event
	class AETHER_API Event
	{
		friend class EventDispatcher;
	public:
		// Must be implemented by all events, use above macros to save time!
		virtual EventType GetEventType() const = 0;
		virtual int GetCategories()	  const = 0;
		virtual const char* GetName() const = 0;

		// Event debugging - overridable so that derived events can give more specific detail as needed
		virtual std::string ToString() const { return GetName(); }

		// Helper to check what if a given event falls under a specific category
		inline bool FallsUnderCategory(EventCategory cat)
		{
			// Nifty line that returns the bitfield of categories and uses "&" to compare if the given bit is within
			return GetCategories() & cat;
		}
	protected:
		bool m_EventHandled = false;
	};

	// Dispatches events, correctly getting the event function for each type of event
	class EventDispatcher
	{
		// Quick helper to make it clearer when the event is being fired off
		template <typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		template <typename T>
		bool Dispatch(EventFn<T> func)
		{
			// Little type check to ensure that we're firing off a valid event
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				// Fire event and store result in the handled flag
				m_Event.m_EventHandled = func(*(T*)&m_Event);				// Ugly looking line but it does this: 
																				//	*(T*)&m_Event casts the m_Event reference from an Event to T. 
																				//	The order of operations is& then(T*) then * .
																				// 	First, & m_Event gets the memory address of of m_Event (now it's type Event*).
																				// 	Second, this Event * gets casted to a T * using (T*) (now it's type T*).
																				// 	Last, this T* gets dereferenced into a T with* (object from second step).
				return true;												// In effect, we have done this : 	func(static_cast<T&>(m_Event)) - but in a way that will actually compile with some naught C hacks

			}
			return false;	// Return false if we hae a type mismatch
		}

	private:
		Event& m_Event;
	};

	// Out stream operator for our logging library to be able to easily output event stuff!
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

	inline std::string format_as(const Event& e)
	{
		return e.ToString();
	}
}