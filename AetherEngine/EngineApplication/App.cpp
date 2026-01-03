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
		// Define where and how we want to draw using transform helper
		Aether::Transform trans;
		trans.m_Scale = { 1.75f, 1.75f, 1.f };
		trans.m_Position = { 0.f, 0.f, 2.0f };

		Aether::Renderer::DrawQuad(trans, Aether::Colours::kYellow);

		// Can re-use transform for another draw!
		trans.m_Scale = { 1.5f, 1.5f, 1.f };
		trans.m_Position = { 0.3f, 0.3f, 1.0f };
		Aether::Renderer::DrawTriangle(trans, Aether::Colours::kPureGreen);
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