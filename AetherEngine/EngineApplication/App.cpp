//===============================================================================
// desc: This represents an application that was created using the engine. This is where game code will go!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <Aether.h>
#include "glm/gtc/matrix_transform.hpp"
//===============================================================================

class DemoLayer : public Aether::Layer
{
public:
	DemoLayer()
		: Layer("Demo") {}

	void OnAttach() override
	{

	}

	void OnEvent(Aether::Event& event) override { /*AETHER_TRACE("{0}", event);*/ }

	void Update() override 
	{
		if (Aether::Input::IsKeyPressed(Aether::KeyCode::kSpace))
			AETHER_INFO("Space is pressed! (Our polling!)"); 
	}

	void Render() override
	{
		// Create identity mat
		glm::mat4 transform = glm::mat4(1.f);
		
		// Scale
		transform = glm::scale(transform,glm::vec3(1.75f, 1.75f, 0.f));
		glm::vec4 colour = glm::vec4({0.5f, 0.5f, 0.f, 1.f});
		Aether::Renderer::DrawQuad(transform, colour);
	}
};

class AetherGame : public Aether::Application
{
public:
	AetherGame() { PushLayer(new DemoLayer()); }
	~AetherGame() {}
};

Aether::Application* Aether::CreateApplication()
{
	return new AetherGame();
}