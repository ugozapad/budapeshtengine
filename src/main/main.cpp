#include "pch.h"

#include "engine/engine_api.h"
#include "engine/allocator.h"
#include "engine/filesystem.h"
#include "engine/input_system.h"
#include "engine/engine.h"

#include "editor/ieditorsystem.h"

extern "C" {
#include "render/microui_render.h"
}

class Main {
public:
	Main() :
		m_engine(nullptr)
	{
	}

	int Init(int argc, char* argv[]);
	void Shutdown();

	void Update();
	void OnEvent(SDL_Event& event);

	Engine* GetEngine() { return m_engine; }

private:
	Engine* m_engine;
	HANDLE m_locker_mutex;
};

int Main::Init(int argc, char* argv[]) {

	bool createLockerMutex = true;
	bool fullscreen = false;
	bool map_load = false;
	const char* map_name = nullptr;

	for (int i = 0; i < argc; i++) {
		if (strcmp("-multinstance", argv[i]) == 0)
			createLockerMutex = false;
		if (strcmp("-fullscreen", argv[i]) == 0)
			fullscreen = true;
		if (strcmp("-map", argv[i]) == 0) {
			if (i == argc) {
				MessageBoxA(NULL, "Missing map name with -map parameter!", "Invalid command line parameter", MB_OK);
			} else {
				map_load = true;
				map_name = argv[i + 1];
			}
		}
	}

	if (createLockerMutex) {
		m_locker_mutex = OpenMutexA(READ_CONTROL, FALSE, "execution_locker_mutex");
		if (m_locker_mutex) {
			MessageBoxA(nullptr, "Failed to open second instance.", "Error", MB_OK | MB_ICONERROR);
			return 2;
		} else {
			m_locker_mutex = CreateMutexA(NULL, FALSE, "execution_locker_mutex");
		}
	}

	m_engine = mem_new<Engine>();
	m_engine->Create(1024, 768, fullscreen);

	if (map_load)
		Engine_NewGame(map_name);

	return 0;
}

void Main::Shutdown() {
	m_engine->Shutdown();
	SAFE_DELETE(m_engine);

	CloseHandle(m_locker_mutex);
}
	
void Main::Update() {
	if (g_input_system->IsKeyPressed(SDL_SCANCODE_ESCAPE))
		m_engine->RequestExit();
	
	m_engine->Update();
}

void Main::OnEvent(SDL_Event& event)
{
	if (m_engine->GetEditorSystem())
		m_engine->GetEditorSystem()->PollEvents(event);

	switch (event.type)
	{
	case SDL_QUIT: m_engine->RequestExit(); break;
	case SDL_KEYDOWN:
		g_input_system->OnKeyDown(event.key.keysym.scancode);
		break;
	case SDL_KEYUP:
		g_input_system->OnKeyUp(event.key.keysym.scancode);
		break;
	case SDL_MOUSEBUTTONDOWN:
		g_input_system->OnMouseKeyDown(event.button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		g_input_system->OnMouseKeyUp(event.button.button);
		break;
	case SDL_MOUSEMOTION:
		g_input_system->OnMouseMove(event.motion.x, event.motion.y);
		break;
	case SDL_MOUSEWHEEL:
		g_input_system->OnMouseWheel(event.wheel.x, event.wheel.y);
		break;
	case SDL_WINDOWEVENT:
	{
		switch (event.window.type)
		{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			m_engine->OnWindowSizeChanged(
				event.window.data1,
				event.window.data2
			);
			break;
		}
	}break;
	}
}

int main(int argc, char* argv[]) {
	Main main;
	int retval = main.Init(argc, argv);

	Engine* pEngine = main.GetEngine();
	while (!pEngine->IsExitRequested()) {
		SDL_Event event;
		while (SDL_PollEvent(&event))
			main.OnEvent(event);

		main.Update();
	}

	main.Shutdown();

	return 0;
}
