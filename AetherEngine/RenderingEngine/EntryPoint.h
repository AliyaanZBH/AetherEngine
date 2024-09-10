#pragma once
//===============================================================================
// desc: Entry point to the engine from applications built using Aether
// auth: Aliyaan Zulfiqar
//===============================================================================

#ifdef AETHER_PLATFORM_WINDOWS
	extern Aether::Application* Aether::CreateApplication();
#endif


int main(int argc, char** argv)
{
	auto game = Aether::CreateApplication();
	game->Run();
	delete game;
	return 0;
}