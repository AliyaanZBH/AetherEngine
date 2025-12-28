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
		// Create verts - position, colour
		// Clockwise verts! Clockwise winding order!
		Aether::Vertex appVerts[] =
		{
			{ {	-0.9f,		-0.9f,		0.8f,	1.f	}, {1.f, 0.f, 0.f, 1.f} },	// Bottom Left
			{ {	-0.9f,		 0.9f,		0.8f,	1.f	}, {0.f, 1.f, 0.f, 1.f} },	// Top Left
			{ {	0.9f,		 0.9f,		0.8f,	1.f	}, {0.f, 0.f, 1.f, 1.f} },	// Top Right
			{ {	0.9f,		-0.9f,		0.8f,	1.f	}, {0.f, 1.f, 1.f, 1.f} }	// Bottom Right
		};

		Aether::BufferDesc AppVbDesc
		{
			.m_Data = appVerts,
			.m_SizeInBytes = sizeof(appVerts),
			.m_Type = Aether::eBufferType::kVertex,
			.m_CPUVisible = true
		};
		

		m_VertexBuffer = Aether::Application::Get().GetRenderer().CreateBuffer(AppVbDesc);
		m_VertexBuffer->Upload(AppVbDesc.m_Data, AppVbDesc.m_SizeInBytes);

		m_VertBufView.m_Buffer = m_VertexBuffer;
		m_VertBufView.m_Stride = sizeof(Aether::Vertex);
		m_VertBufView.m_Offset = 0;


		//unsigned int indices[] = { 0, 1, 2 };
		unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

		Aether::BufferDesc ibDesc
		{
			.m_Data = indices,
			.m_SizeInBytes = sizeof(indices),
			.m_Type = Aether::eBufferType::kIndex,
			.m_CPUVisible = true
		};		
		
		m_IndexBuffer = Aether::Application::Get().GetRenderer().CreateBuffer(ibDesc);
		m_IndexBuffer->Upload(ibDesc.m_Data, ibDesc.m_SizeInBytes);
		m_IndBufView.m_Buffer = m_IndexBuffer;
		m_IndBufView.m_Count = 6;
		m_IndBufView.m_IndexSize = sizeof(unsigned int);
		m_IndBufView.m_Offset = 0;

		// Finalise our upload to the renderer
		Aether::Application::Get().GetRenderer().FinalizeUploads();
	}

	void OnEvent(Aether::Event& event) override { /*AETHER_TRACE("{0}", event);*/ }

	void Update() override 
	{
		if (Aether::Input::IsKeyPressed(Aether::KeyCode::kSpace))
			AETHER_INFO("Space is pressed! (Our polling!)"); 
	}

	void Render() override
	{
		Aether::Application::Get().GetRenderer().Render(&m_VertBufView, &m_IndBufView);
	}

private:
	Aether::Buffer* m_VertexBuffer;
	Aether::VertexBufferView m_VertBufView;
	Aether::Buffer* m_IndexBuffer;
	Aether::IndexBufferView m_IndBufView;

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