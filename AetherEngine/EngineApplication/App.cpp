//===============================================================================
// desc: This represents an application that was created using the engine. This is where game code will go!
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <Aether.h>
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
		Aether::Renderer::DrawQuad();
		//Aether::Application::Get().GetRenderer().Render(&m_VertBufView, &m_IndBufView);
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