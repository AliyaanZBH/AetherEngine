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