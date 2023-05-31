#include "engine/allocator.h"
#include "engine/engine.h"
#include "engine/iosdriver.h"
#include "engine/filesystem.h"
#include "engine/input_system.h"
#include "engine/objectfactory.h"
#include "engine/entity.h"
#include "engine/level.h"
#include "engine/level_mesh.h"
#include "engine/player.h"
#include "engine/camera.h"

#include <stdio.h>

#ifndef NDEBUG
#define DBG_STR " Dbg"
#else
#define DBG_STR
#endif // !NDEBUG

Engine* g_engine = nullptr;

void registerEngineStuff()
{
	g_object_factory->registerObject<Entity>();
	g_object_factory->registerObject<LevelMesh>();
	g_object_factory->registerObject<Player>();
}

Engine::Engine() :
	m_bExitRequested(false),
	m_render_window(nullptr),
	m_level(nullptr)
{
	g_engine = this;
}

Engine::~Engine() {
	g_engine = nullptr;
}

void Engine::init(int width, int height, bool fullscreen) {
	if (SDL_Init(SDL_INIT_EVERYTHING ^ SDL_INIT_SENSOR) != 0) {
		printf("Failed to initialize SDL2. Error core: %s\n", SDL_GetError());
	}

	// Initialize OS Driver
	IOsDriver::getInstance()->init();

    // create filesystem
#ifdef ENABLE_PHYSFS
	g_file_system = IFileSystem::createPhysFS();
#else
	g_file_system = IFileSystem::create();
#endif // ENABLE_PHYSFS

	if (!g_file_system->fileExist("data/")) {
		FATAL("ERROR: Not found game data folder.");
	}

    // Initialize OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fullscreen)
        window_flags |= SDL_WINDOW_FULLSCREEN;

    // Create window
    m_render_window = SDL_CreateWindow("Budapesht" DBG_STR, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!m_render_window) {
        printf("Failed to create render window. Error core: %s\n", SDL_GetError());
    }

	// initialize input system
	g_input_system = IInputSystem::create(g_default_allocator);
	g_input_system->init();

	// initialize object factory
	g_object_factory = MEM_NEW(*g_default_allocator, ObjectFactory, *g_default_allocator);

	// register engine objects
	registerEngineStuff();

	// create level
	m_level = MEM_NEW(*g_default_allocator, Level, *g_default_allocator);

	// create renderer
	printf("Creating render device\n");
	m_render_device = createRenderDevice();
	m_render_device->init(m_render_window);

	// init viewport
	m_viewport.x = 0;
	m_viewport.y = 0;
	SDL_GetWindowSize(
		m_render_window,
		&m_viewport.width,
		&m_viewport.height
	);
}

void Engine::update()
{
	g_camera.updateLook(
		m_viewport.width,
		m_viewport.height
	);

	m_render_device->beginPass(m_viewport, PASSCLEAR_COLOR | PASSCLEAR_DEPTH);

	m_level->render();

	m_render_device->endPass();
	m_render_device->commit();

	m_render_device->present(false);
}

void Engine::shutdown() {
	if (m_render_device) {
		m_render_device->shutdown();
		MEM_DELETE(*g_default_allocator, IRenderDevice, m_render_device);
	}

	if (m_level) {
		MEM_DELETE(*g_default_allocator, Level, m_level);
		m_level = nullptr;
	}

	if (g_object_factory) {
		MEM_DELETE(*g_default_allocator, ObjectFactory, g_object_factory);
		g_object_factory = nullptr;
	}

	if (g_input_system) {
		g_input_system->shutdown();
		MEM_DELETE(*g_default_allocator, IInputSystem, g_input_system);
		g_input_system = nullptr;
	}

	if (m_render_window) {
		SDL_DestroyWindow(m_render_window);
		m_render_window = nullptr;
	}

	IFileSystem::destroy(g_file_system);

	SDL_Quit();
}

SDL_Window* Engine::getRenderWindow() {
	return m_render_window;
}

IInputSystem* Engine::getInputSystem() {
	return g_input_system;
}

Level* Engine::getLevel()
{
	return m_level;
}

void Engine::onWindowSizeChanged(uint32_t w, uint32_t h)
{
	m_viewport.width	= w;
	m_viewport.height	= h;
}
