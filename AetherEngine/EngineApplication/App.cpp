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

		// Define our materials
		m_CubeMaterial = new Aether::MaterialSolidColour(Aether::Colours::kYellow);
		m_QuadMaterial = new Aether::MaterialSolidColour(Aether::Colours::kDarkBlue);
		m_TriMaterial = new Aether::MaterialSolidColour(Aether::Colours::kCyan);
	}

	void OnDetach() override
	{
		delete m_CameraController;

		delete m_CubeMaterial;
		delete m_QuadMaterial;
		delete m_TriMaterial;
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
		Aether::Renderer::Draw(Aether::eDrawGeoType::kCube, m_CubeTransform, m_CubeMaterial);

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
					m_QuadMaterial->SetColour(Aether::Colours::kDarkBlue);
				else
					m_QuadMaterial->SetColour(Aether::Colours::kDarkRed);

				Aether::Renderer::Draw(Aether::eDrawGeoType::kQuad, trans, m_QuadMaterial);
			}
		}

		// Can re-use local transform for another draw!
		trans.m_Scale = { 1.75f, 1.75f, 1.f };
		trans.m_Position = { 0.3f, 0.3f, 1.0f };
		Aether::Renderer::Draw(Aether::eDrawGeoType::kTri, trans, m_TriMaterial);
	}

private:
	Aether::CameraController* m_CameraController;

	Aether::Transform m_CubeTransform;
	Aether::MaterialSolidColour* m_CubeMaterial;
	Aether::MaterialSolidColour* m_QuadMaterial;
	Aether::MaterialSolidColour* m_TriMaterial;
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