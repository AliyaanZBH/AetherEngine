#pragma once
//===============================================================================
// desc: A wrapper class for spdlog, that will be exported for use by any application using Aether
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "spdlog/spdlog.h"
//===============================================================================

namespace Aether
{
	class AETHER_API Log
	{
	public:
		Log();
		~Log();

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger}
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger}

	private:
		std::shared_ptr<spdlog::logger> s_CoreLogger;
		std::shared_ptr<spdlog::logger> s_ClientLogger;

	};
}