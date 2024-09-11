#pragma once
//===============================================================================
// desc: Entry point to the engine from applications built using Aether
// auth: Aliyaan Zulfiqar
//===============================================================================

#ifdef AETHER_PLATFORM_WINDOWS
	extern Aether::Application* Aether::CreateApplication();
#endif

// Using args for Win32 support down the line
int main(int argc, char** argv)
{
	// Spin up logger
	Aether::Log::Init();
	// Test
	Aether::Log::GetAetherLogger()->warn("Initalised Log!");
	Aether::Log::GetClientLogger()->info("Hello AetherApp!");

	// Register this client application with the core Aether engine
	auto game = Aether::CreateApplication();
	// Run our game!
	game->Run();
	delete game;
	return 0;
}