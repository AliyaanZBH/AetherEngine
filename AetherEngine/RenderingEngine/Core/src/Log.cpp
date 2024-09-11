//===============================================================================
// desc: A wrapper class for spdlog, that will be exported for use by any application using Aether
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
//===============================================================================

std::shared_ptr<spdlog::logger> Aether::Log::s_AetherLogger;
std::shared_ptr<spdlog::logger> Aether::Log::s_ClientLogger;


void Aether::Log::Init()
{
	// Set global log formatting - print log level and timestamp, name of the logger (Aether or Client), and then the actual message, followed by the source file and line.
	// It will be wrapped in a colour based on log level.
	spdlog::set_pattern("%^[%n] [%T] %v%@%$");

	// Setup core logger with multithreaded colour and set level to trace
	s_AetherLogger = spdlog::stdout_color_mt("AETHER");
	s_AetherLogger->set_level(spdlog::level::trace);

	// Repeat for client
	s_ClientLogger = spdlog::stdout_color_mt("APP");
	s_ClientLogger->set_level(spdlog::level::trace);
}