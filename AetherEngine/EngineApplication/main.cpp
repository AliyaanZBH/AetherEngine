//===============================================================================
// desc: This represents an application that was created using the engine. This is where game code will go!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <Aether.h>
//===============================================================================

class AetherGame : public Aether::Application
{
	public:
		AetherGame()
		{}
		~AetherGame()
		{}
};

Aether::Application* Aether::CreateApplication()
{
	return new AetherGame();
}