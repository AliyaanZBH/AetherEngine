#pragma once
//===============================================================================
// desc: Entry point to the engine from applications built using Aether
// auth: Aliyaan Zulfiqar
//===============================================================================

// Using args for Win32 support down the line, this will still work with GLFW in the mean time.
#ifdef AETHER_PLATFORM_WINDOWS

extern Aether::Application* Aether::CreateApplication();

int main(int argc, char** argv)
{
	// Spin up logger
	Aether::Log::Init();

	// Test logging with variadic macros!
	std::string name("NewGame");
	int var = 5;
	AETHER_CORE_WARN("Initalised Log!");

	std::string renderAPI = "";
#ifdef USE_DX11
	renderAPI = "DirectX 11";
#endif
#ifdef USE_DX12
	renderAPI = "DirectX 12";
#endif
	AETHER_CORE_INFO("Using Renderer: {0}", renderAPI);
	
	AETHER_TRACE("Hello from an AetherApp! Var={0}", var);
	AETHER_TRACE("This game is called: {0}", name);

	// Register this client application with the core Aether engine
	auto game = Aether::CreateApplication();
	// Run our game!
	game->Run();
	delete game;
	return 0;
}

#endif