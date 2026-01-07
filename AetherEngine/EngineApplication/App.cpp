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
		: Layer("Demo") 
	{
		m_CameraController = new Aether::CameraController(*Aether::Application::Get().GetCamera());
	}

	void OnAttach() override
	{
		// Define where and how we want to draw using a dedicated transform that we can update!
		m_CubeTransform.m_Scale = { 2.0f, 2.0f, 2.f };
		m_CubeTransform.m_Position = { -2.f, 0.f, 2.0f };
	}

	void OnDetach() override
	{
		delete m_CameraController;
	}

	void OnEvent(Aether::Event& event) override
	{
		/*AETHER_TRACE("{0}", event);*/
		m_CameraController->OnEvent(event);
	}

	void Update() override 
	{
		if (Aether::Input::IsKeyPressed(Aether::KeyCode::kSpace))
			AETHER_INFO("Space is pressed! (Our polling!)"); 

		m_CameraController->Update(Aether::Time::GetDT());

		// Update rotation
		m_CubeTransform.m_Rotation.y += Aether::Time::GetDT();
	}

	void Render() override
	{
		Aether::Renderer::DrawCube(m_CubeTransform, Aether::Colours::kYellow);

		// Define where and how we want to draw using a local transform helper
		Aether::Transform trans;
		trans.m_Scale = { 0.75f, 0.75f, 1.f };

		// Draw grid - temp so magic numbers for now
		for (int y = -5; y < 10; ++y)
		{
			for (int x = -5; x < 10; x++)
			{
				trans.m_Position = { x * 0.85f, y * 0.85f, 4.f };

				// Alternate colours
				if (x % 2 == 0)
					Aether::Renderer::DrawQuad(trans, Aether::Colours::kDarkBlue);
				else
					Aether::Renderer::DrawQuad(trans, Aether::Colours::kDarkRed);
			}
		}

		// Can re-use local transform for another draw!
		trans.m_Scale = { 1.75f, 1.75f, 1.f };
		trans.m_Position = { 0.3f, 0.3f, 1.0f };
		Aether::Renderer::DrawTriangle(trans, Aether::Colours::kCyan);
	}

private:
	Aether::CameraController* m_CameraController;

	Aether::Transform m_CubeTransform;
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