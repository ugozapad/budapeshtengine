#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "engine/allocator.h"
#include "engine/engine.h"
#include "render/render.h"

class Main {
public:
	Main() :
		m_engine(nullptr)
	,	m_render(nullptr)
	{
	}

	int init(int argc, char* argv[]);
	void shutdown();

	void update();

private:
	Engine* m_engine;
	IRender* m_render;
};

static Main s_main;
static HANDLE s_locker_mutex;

int Main::init(int argc, char* argv[]) {

	bool createLockerMutex = true;

	for (int i = 0; i < argc; i++) {
		if (strcmp("-multinstance", argv[i]) == 0)
			createLockerMutex = false;
	}

	if (createLockerMutex) {
		s_locker_mutex = OpenMutexA(READ_CONTROL, FALSE, "execution_locker_mutex");
		if (s_locker_mutex) {
			MessageBoxA(nullptr, "Failed to open second instance.", "Error", MB_OK | MB_ICONERROR);
			return 2;
		} else {
			s_locker_mutex = CreateMutexA(NULL, FALSE, "execution_locker_mutex");
		}
	}

	g_default_allocator = createDefaultAllocator();

	m_engine = NEW(*g_default_allocator, Engine);
	m_engine->init();

	m_render = createRender();
	m_render->init(m_engine->getRenderWindow());

	return 0;
}

void Main::shutdown() {
	m_render->shutdown();
	DELETE(*g_default_allocator, IRender, m_render);

	m_engine->shutdown();
	DELETE(*g_default_allocator, Engine, m_engine);
}

void Main::update()
{
	m_render->renderFrame();
}

int main(int argc, char* argv[]) {

	int retval = s_main.init(argc, argv);

	bool run = true;
	while (run) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				run = false;
			}
		}

		s_main.update();
	}

	s_main.shutdown();

	return 0;
}

