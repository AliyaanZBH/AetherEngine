#pragma once
//===============================================================================
// desc: A wrapper class for spdlog, that will be exported for use by any application using Aether
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"
//===============================================================================

namespace Aether
{
	class AETHER_API Log
	{
	public:
		Log() {}
		~Log() {}

		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetAetherLogger() { return s_AetherLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		
	private:
		// The loggers are static so that they persist for each other
		static std::shared_ptr<spdlog::logger> s_AetherLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//
// Some macros to make logging easier to use
//

// Variadic macro so that logging can still be highly customisable when used through the macro
// These ones are for the engine
#define AETHER_CORE_TRACE(...)  Aether::Log::GetAetherLogger()->trace(__VA_ARGS__)
#define AETHER_CORE_INFO(...)   Aether::Log::GetAetherLogger()->info(__VA_ARGS__)
#define AETHER_CORE_WARN(...)   Aether::Log::GetAetherLogger()->warn(__VA_ARGS__)
#define AETHER_CORE_ERROR(...)  Aether::Log::GetAetherLogger()->error(__VA_ARGS__)
#define AETHER_CORE_FATAL(...)  Aether::Log::GetAetherLogger()->fatal(__VA_ARGS__)

// These ones for debugging inside of games
#define AETHER_TRACE(...)  Aether::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AETHER_INFO(...)   Aether::Log::GetClientLogger()->info(__VA_ARGS__)
#define AETHER_WARN(...)   Aether::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AETHER_ERROR(...)  Aether::Log::GetClientLogger()->error(__VA_ARGS__)
#define AETHER_FATAL(...)  Aether::Log::GetClientLogger()->fatal(__VA_ARGS__)