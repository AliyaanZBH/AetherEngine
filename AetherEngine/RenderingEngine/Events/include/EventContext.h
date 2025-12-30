#pragma once
//===============================================================================
// desc: Small context helper for systems that need some visibility on the event system
// auth: Aliyaan Zulfiqar
//===============================================================================
class Event;

using EventCallbackFn = std::function<void(Event&)>;
