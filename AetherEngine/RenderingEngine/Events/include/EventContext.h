#pragma once
//===============================================================================
// desc: Small context helper for systems that need some visibility on the event system. Ensure this is included AFTER derived events, got a nasty bug with this otherwise
// auth: Aliyaan Zulfiqar
//===============================================================================

namespace Aether
{
	class Event;

	// Void function that takes an event reference as a parameter
	using EventCallbackFn = std::function<void(Event&)>;
}