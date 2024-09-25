#pragma once
//===============================================================================
// desc: The main file that ties the whole Event System together
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <string>
#include <functional>	// TODO: Move these to PCH

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
		KeyPressed, KeyReleased,
		MouseMoved, MouseClicked, MouseClickReleased, MouseScrolled,
		WindowClose, WindowMoved, WindowResize, WindowFocus, WindowLostFocus,
		AppUpdate, AppRender, AppTick
	};

	// Helps with filtering events, for example when ordering our event queue
	// Using a bit field as events can fall under multiple categories
	enum class EventCategory
	{
		None = 0,
		Application = BIT(0),
		Input		= BIT(1),
		Mouse		= BIT(2),		// For movement and scrolling
		Click		= BIT(3),		// For clicking LMB etc.
		Keyboard    = BIT(4),
	};

	//
	// Macros to allow quick event creation
	//

	// The static types are needed so that we can read key events without being tied to a specific instance of the event class.
#define EVENT_CLASS_TYPE(type)					static EventType GetStaticType()				{ return EventType::##type; }\
											virtual EventType GetEventType() const override { return GetStaticType(); }\
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
		virtual std::string ToString() const { GetName(); }

		// Helper to check what if a given event falls under a specific category
		inline bool FallsUnderCategory(EventCategory cat)
		{
			// Nifty line that returns the bitfield of categories and uses "&" to compare if the given bit is within
			return GetCategories() & cat;
		}
	protected:
		bool m_EventHandled = false;
	};

	class EventDispatcher
	{

	};
}