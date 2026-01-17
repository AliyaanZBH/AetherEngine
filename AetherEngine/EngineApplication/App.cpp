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
		m_CubeSmoothTransform.m_Scale = { 0.5f, 0.5f, 0.5f };
		m_CubeSmoothTransform.m_Position = { -1.f, 0.f, 1.0f };
		
		// Setup material instances, these are attached to objects and represent existing material pipelines
		m_CubeSmoothMaterial = new Aether::MaterialInstance(Aether::eMaterialType::kBlinnPhong);
		// Set the data we want!
		auto& cubeMatData = m_CubeSmoothMaterial->GetData<Aether::LitColourMaterialData>();
		// Instances have default data, e.g. for colours the default is Aether gold. Just set roughness as that's all we care about here
		cubeMatData.m_Roughness = 0.f;

		// Remember to upload!
		m_CubeSmoothMaterial->Upload();


		m_CubeRoughTransform.m_Scale = { 0.5f, 0.5f, 0.5f };
		m_CubeRoughTransform.m_Position = { 1.f, 0.f, 1.0f };
		m_CubeRoughMaterial = new Aether::MaterialInstance(Aether::eMaterialType::kBlinnPhong);
		cubeMatData = m_CubeRoughMaterial->GetData<Aether::LitColourMaterialData>();
		cubeMatData.m_Roughness = 1.0f;
		m_CubeRoughMaterial->Upload();

		m_CubeAverageTransform.m_Scale = { 0.5f, 0.5f, 0.5f };
		m_CubeAverageTransform.m_Position = { 0.f, 1.f, 1.0f };
		m_CubeAverageMaterial = new Aether::MaterialInstance(Aether::eMaterialType::kBlinnPhong);
		cubeMatData = m_CubeAverageMaterial->GetData<Aether::LitColourMaterialData>();
		cubeMatData.m_Roughness = 0.5f;
		m_CubeAverageMaterial->Upload();

		// For demo purposes
		m_QuadMaterials.reserve(225);
		// Setup grid - temp so magic numbers for now
		for (int y = 0; y < 15; ++y)
		{
			for (int x = 0; x < 15; x++)
			{
				const int idx = x + y * 15;
				Aether::MaterialInstance matInst = Aether::MaterialInstance(Aether::eMaterialType::kSolidColour);

				// Alternate colours like a checkerboard
				if ((x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1))
					matInst.GetData<Aether::FlatColourMaterialData>().m_Colour = Aether::Colours::kDarkBlue;
				else
					matInst.GetData<Aether::FlatColourMaterialData>().m_Colour = Aether::Colours::kDarkRed;

				m_QuadMaterials.push_back(matInst);
				m_QuadMaterials[idx].Upload();
			}
		}

		m_TriMaterial = new Aether::MaterialInstance(Aether::eMaterialType::kSolidColour);
		m_TriMaterial->GetData<Aether::FlatColourMaterialData>().m_Colour = Aether::Colours::kCyan;
		m_TriMaterial->Upload();

	}
	void OnDetach() override
	{
		delete m_CameraController;

		delete m_CubeRoughMaterial;
		delete m_CubeSmoothMaterial;
		delete m_CubeAverageMaterial;
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

		// Update rotations
		m_CubeSmoothTransform.m_Rotation.y += Aether::Time::GetDT() * 0.75f;
		m_CubeRoughTransform.m_Rotation.y -= Aether::Time::GetDT() * 0.75f;
		m_CubeAverageTransform.m_Rotation.z += Aether::Time::GetDT() * 0.75f;
	}

	void Render() override
	{
		Aether::Renderer::Draw(Aether::eDrawGeoType::kCube, m_CubeSmoothTransform, m_CubeSmoothMaterial);
		Aether::Renderer::Draw(Aether::eDrawGeoType::kCube, m_CubeRoughTransform, m_CubeRoughMaterial);
		Aether::Renderer::Draw(Aether::eDrawGeoType::kCube, m_CubeAverageTransform, m_CubeAverageMaterial);

		// Define where and how we want to draw using a local transform helper
		Aether::Transform trans;
		trans.m_Scale = { 0.75f, 0.75f, 1.f };

		// Draw grid - temp so magic numbers for now
		for (int y = 0; y < 15; ++y)
		{
			for (int x = 0; x < 15; x++)
			{
				trans.m_Position = { (x - 5.f) * 0.85f, (y - 5.f) * 0.85f, 4.f };
				const int idx = x + y * 15;
				Aether::Renderer::Draw(Aether::eDrawGeoType::kQuad, trans, &m_QuadMaterials[idx]);
			}
		}


		// Can re-use local transform for another draw!
		trans.m_Scale = { 1.75f, 1.75f, 1.f };
		trans.m_Position = { 3.f, 3.f, 3.0f };
		Aether::Renderer::Draw(Aether::eDrawGeoType::kTri, trans, m_TriMaterial);
	}

private:
	Aether::CameraController* m_CameraController;

	Aether::Transform m_CubeRoughTransform;
	Aether::Transform m_CubeSmoothTransform;
	Aether::Transform m_CubeAverageTransform;

	// Material instances that can be of any material type
	Aether::MaterialInstance* m_CubeRoughMaterial;
	Aether::MaterialInstance* m_CubeSmoothMaterial;
	Aether::MaterialInstance* m_CubeAverageMaterial;

	Aether::MaterialInstance* m_TriMaterial;

	// We can even store large containers of them
	std::vector<Aether::MaterialInstance> m_QuadMaterials;
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