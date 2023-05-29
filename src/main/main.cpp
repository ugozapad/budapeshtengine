#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdint.h>
#include <stdio.h>

#include "engine/allocator.h"
#include "engine/filesystem.h"
#include "engine/input_system.h"
#include "engine/engine.h"
#include "engine/level.h"
#include "engine/camera.h"
#include "render/irenderdevice.h"
#include "render/texture.h"
#include "engine/shader_engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
#include "render/microui_render.h"
}

class Main {
public:
	Main() :
		m_engine(nullptr)
	,	m_render_device(nullptr)
	{
	}

	int init(int argc, char* argv[]);
	void shutdown();

	void update();

	Engine* getEngine() { return m_engine; }

private:
	Engine* m_engine;
	IRenderDevice* m_render_device;
};

static Main s_main;
static HANDLE s_locker_mutex;

int Main::init(int argc, char* argv[]) {

	bool createLockerMutex = true;
	bool fullscreen = false;

	for (int i = 0; i < argc; i++) {
		if (strcmp("-multinstance", argv[i]) == 0)
			createLockerMutex = false;
		if (strcmp("-fullscreen", argv[0]) == 0)
			fullscreen = true;
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

	m_engine = MEM_NEW(*g_default_allocator, Engine);
	m_engine->init(1024, 768, fullscreen);

	printf("Creating render device\n");
	m_render_device = createRenderDevice();
	m_render_device->init(m_engine->getRenderWindow());

	g_pShaderEngine = MEM_NEW(*g_default_allocator, ShaderEngine, "gl33");

	m_engine->getLevel()->load("test_baking");

	return 0;
}

void Main::shutdown() {
	MEM_DELETE(*g_default_allocator, ShaderEngine, g_pShaderEngine);

	m_render_device->shutdown();
	MEM_DELETE(*g_default_allocator, IRenderDevice, m_render_device);

	m_engine->shutdown();
	MEM_DELETE(*g_default_allocator, Engine, m_engine);
}

static glm::mat4 s_mat4_idenitity = glm::mat4(1.0f);

void Main::update() {
	if (g_input_system->isKeyPressed(SDL_SCANCODE_ESCAPE))
		m_engine->requestExit();

	int width = 0, height = 0;
	SDL_GetWindowSize(m_engine->getRenderWindow(), &width, &height);

	g_camera.updateLook(width, height);

	viewport_t viewport = { 0,0,width,height };
	m_render_device->beginPass(viewport, PASSCLEAR_COLOR | PASSCLEAR_DEPTH);

	m_engine->getLevel()->render();

	m_render_device->endPass();
	m_render_device->commit();

	m_render_device->present(false);
}

int main(int argc, char* argv[]) {

	int retval = s_main.init(argc, argv);

	Engine* pEngine = s_main.getEngine();
	while (!pEngine->isExitRequested()) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			switch (event.type)
			{
			case SDL_QUIT: pEngine->requestExit(); break;
			case SDL_KEYDOWN:
				g_input_system->onKeyDown(event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				g_input_system->onKeyUp(event.key.keysym.scancode);
				break;
			case SDL_MOUSEBUTTONDOWN:
				g_input_system->onMouseKeyDown(event.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				g_input_system->onMouseKeyUp(event.button.button);
				break;
			case SDL_MOUSEMOTION:
				g_input_system->onMouseMove(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEWHEEL:
				g_input_system->onMouseWheel(event.wheel.x, event.wheel.y);
				break;
			}
		}

		s_main.update();
	}

	s_main.shutdown();

	return 0;
}

