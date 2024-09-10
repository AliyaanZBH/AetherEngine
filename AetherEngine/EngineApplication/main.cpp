#include <Aether.h>


class EditorApp : public Aether::Application
{
	public:
		EditorApp()
		{}
		~EditorApp()
		{}
};

int main()
{
	EditorApp* app = new EditorApp();
	app->Run();
	delete app;
}