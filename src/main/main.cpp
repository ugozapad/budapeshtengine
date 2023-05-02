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
	m_render_device->shutdown();
	MEM_DELETE(*g_default_allocator, IRenderDevice, m_render_device);

	m_engine->shutdown();
	MEM_DELETE(*g_default_allocator, Engine, m_engine);
}

static glm::mat4 s_mat4_idenitity = glm::mat4(1.0f);

void Main::update() {
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

			if (event.type == SDL_KEYDOWN) {
				inputEvent_t input_event = {};
				input_event.type = INPUT_EVENT_KEY_DOWN;
				input_event.scancode = event.key.keysym.scancode;
				g_input_system->pushEvent(input_event);
			} else if (event.type == SDL_KEYUP) {
				inputEvent_t input_event = {};
				input_event.type = INPUT_EVENT_KEY_UP;
				input_event.scancode = event.key.keysym.scancode;
				g_input_system->pushEvent(input_event);
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				inputEvent_t input_event = {};
				input_event.type = INPUT_EVENT_MOUSE_BUTTON_DOWN;
				input_event.mousebutton = event.button.button;
				g_input_system->pushEvent(input_event);
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				inputEvent_t input_event = {};
				input_event.type = INPUT_EVENT_MOUSE_BUTTON_UP;
				input_event.mousebutton = event.button.button;
				g_input_system->pushEvent(input_event);
			}
		}

		s_main.update();
	}

	s_main.shutdown();

	return 0;
}

