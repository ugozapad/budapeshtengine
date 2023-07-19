#include "pch.h"
#include "engine/allocator.h"
#include "engine/engine.h"
#include "engine/iosdriver.h"
#include "engine/filesystem.h"
#include "engine/logger.h"
#include "engine/input_system.h"
#include "engine/objectfactory.h"
#include "engine/entity.h"
#include "engine/level.h"
#include "engine/level_mesh.h"
#include "engine/player.h"
#include "engine/camera.h"
#include "engine/material_system.h"
#include "engine/sound_system.h"

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
	m_level(nullptr),
	m_render_device(nullptr),
	m_viewport{0}
{
	g_engine = this;
}

Engine::~Engine() {
	g_engine = nullptr;
}

void Engine::init(int width, int height, bool fullscreen)
{
	if (SDL_Init(SDL_INIT_EVERYTHING ^ SDL_INIT_SENSOR) != 0)
	{
		Msg("Failed to initialize SDL2. Error core: %s\n", SDL_GetError());
	}

	// Initialize OS Driver
	IOsDriver::getInstance()->init();

	// create filesystem
//#ifdef ENABLE_PHYSFS
//	g_file_system = IFileSystem::createPhysFS();
//#else
//	g_file_system = IFileSystem::create();
//#endif // ENABLE_PHYSFS

	g_file_system = IFileSystem::create();

	// Create logger
	logOpen("engine");

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
		Msg("Failed to create render window. Error core: %s", SDL_GetError());
	}

	// initialize input system
	g_input_system = IInputSystem::create(g_allocator);
	g_input_system->init();

	// initialize sound system
	createSoundSystem("sound");

	// initialize object factory
	g_object_factory = new ObjectFactory();

	// register engine objects
	registerEngineStuff();

	// create level
	m_level = new Level();

	// create renderer
	Msg("Creating render device");
	m_render_device = createRenderDevice();
	m_render_device->init(m_render_window);

	g_material_system.Init();

	// init viewport
	m_viewport.x = 0;
	m_viewport.y = 0;
	SDL_GetWindowSize(
		m_render_window,
		&m_viewport.width,
		&m_viewport.height
	);
}

void Engine::createSoundSystem(const char* soundname)
{
	ISoundSystem* pSoundSystem = NULL;
	char buff[_MAX_PATH]; snprintf(buff, sizeof(buff), "%s.dll", soundname);
	HMODULE hSoundSystemLib = LoadLibrary(buff);
	if (hSoundSystemLib != NULL)
	{
		createSoundSystem_t createSoundSystemProc = (createSoundSystem_t)GetProcAddress(hSoundSystemLib, "createSoundSystem");
		if (createSoundSystemProc != NULL)
		{
			pSoundSystem = createSoundSystemProc();
		}
		// Dima : Do we need to close it? 
		// Kirill: No
		//CloseHandle(hSoundSystemLib);
	}

	ASSERT(pSoundSystem && "Failed to load sound sys. Missing dll or OpenAL installation");
	g_pSoundSystem = pSoundSystem;
}

void Engine::update()
{
	g_camera.updateLook(
		m_viewport.width,
		m_viewport.height
	);

	g_pSoundSystem->update(0.0f);

	m_render_device->beginPass(m_viewport, PASSCLEAR_COLOR | PASSCLEAR_DEPTH);

	m_level->render();

	m_render_device->endPass();
	m_render_device->commit();

	m_render_device->present(false);
}

void Engine::shutdown()
{
	g_material_system.Shutdown();
	
	if (m_render_device) {
		m_render_device->shutdown();
		SAFE_DELETE(m_render_device);
	}

	if (m_level) {
		SAFE_DELETE(m_level);
	}

	if (g_object_factory) {
		SAFE_DELETE(g_object_factory);
	}

	if (g_input_system) {
		g_input_system->shutdown();
		SAFE_DELETE(g_input_system);
	}

	if (g_pSoundSystem) {
		SAFE_DELETE(g_pSoundSystem);
	}

	if (m_render_window) {
		SDL_DestroyWindow(m_render_window);
		m_render_window = nullptr;
	}

	logClose();

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

IRenderDevice* Engine::getRenderDevice()
{
	return m_render_device;
}

void Engine::onWindowSizeChanged(uint32_t w, uint32_t h)
{
	m_viewport.width	= w;
	m_viewport.height	= h;
}
