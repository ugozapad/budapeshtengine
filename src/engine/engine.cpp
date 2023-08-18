#include "pch.h"
#include "engine/allocator.h"
#include "engine/timer.h"
#include "engine/engine.h"
#include "engine/iosdriver.h"
#include "engine/filesystem.h"
#include "engine/varmanager.h"
#include "engine/logger.h"
#include "engine/input_system.h"
#include "engine/objectfactory.h"
#include "engine/entity.h"
#include "engine/level.h"
#include "engine/level_mesh.h"
#include "engine/camera.h"
#include "engine/material_system.h"
#include "engine/render.h"
#include "engine/sound_system.h"
#include "engine/physics_world.h"
#include "engine/igamepersistent.h"

#include "game/gamelib.h"

#include "editor/ieditorsystem.h"

#ifndef NDEBUG
#define DBG_STR " Dbg"
#else
#define DBG_STR
#endif // !NDEBUG

ENGINE_API Engine* g_engine = nullptr;

static Var developer("developer", "0", "", VARFLAG_NOSAVE | VARFLAG_SERVER_PROTECT);
static Var vid_mode("vid_mode", "1024x768", "", VARFLAG_NONE);
static Var creator("creator", "Kirill", "", VARFLAG_NONE);

void registerEngineVars()
{
	g_VarManager.RegisterVar(&developer);
	g_VarManager.RegisterVar(&vid_mode);
	g_VarManager.RegisterVar(&creator);
}

void registerEngineStuff()
{
	g_object_factory->RegisterObject<Entity>("entity");
	g_object_factory->RegisterObject<LevelMesh>("level_mesh");
	g_object_factory->RegisterObject<DynamicMeshEntity>("dynamic_mesh");
}

Engine::Engine() :
	m_bExitRequested(false),
	m_render_window(nullptr),
	m_level(nullptr),
	m_render_device(nullptr),
	m_editor_system(nullptr),
	m_viewport{0}
{
	g_engine = this;
}

Engine::~Engine() {
	g_engine = nullptr;
}

void Engine::Create(int width, int height, bool fullscreen)
{
	if (SDL_Init(SDL_INIT_EVERYTHING ^ SDL_INIT_SENSOR) != 0)
	{
		Msg("Failed to initialize SDL2. Error core: %s\n", SDL_GetError());
	}

	registerEngineVars();

	// Create timer
	GetSystemTimer()->Init();

	// Initialize OS Driver
	IOsDriver::getInstance()->Init();

	// create filesystem
	g_file_system = IFileSystem::Create();

	// Create logger
	LogOpen("engine");

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
	g_input_system = IInputSystem::Create();
	g_input_system->Init();

	// initialize object factory
	g_object_factory = mem_new<ObjectFactory>();

	// register engine objects
	registerEngineStuff();

	// create renderer
	Msg("Creating render device");
	CreateRenderDevice("sokol_rdev");
	m_render_device->init(m_render_window);

	g_render.Init();

	// init viewport
	m_viewport.x = 0;
	m_viewport.y = 0;
	SDL_GetWindowSize(
		m_render_window,
		&m_viewport.width,
		&m_viewport.height
	);

	// initialize sound system
	CreateSoundSystem("sound");

	// Initialize physics world
	PhysicsWorld::StaticInit();

	// create level
	m_level = mem_new<Level>();

	// load game library
	CreateGameLib("game");

	// create game persistent
	ASSERT(!g_pGamePersistent);
	g_pGamePersistent = CREATE_OBJECT(IGamePersistent, CLSID_GAMEPERSISTENT);

	bool is_editor_mode = strstr(GetCommandLineA(), "-editor");
	if (is_editor_mode)
		CreateEditor();

	if (!is_editor_mode)
		g_VarManager.Save("data/default.cfg");

	GetLevel()->Load("test_baking");
}

//////////////////////////////////////////////////////////////////////
// Kirill:	TODO: Make hRenderDeviceLib, hSoundSystemLib, hGameLib
//			as global variables or rewrite to SDL dyn libs loading
//
//			TODO FUTURE: Linking full engine as static library to main
//////////////////////////////////////////////////////////////////////

HMODULE g_hRenderDeviceLib = NULL;
HMODULE g_hSoundSystemLib = NULL;
HMODULE g_hGameLib = NULL;
HMODULE g_hEditorLib = NULL;

void Engine::CreateRenderDevice(const char* devicename)
{
	IRenderDevice* pRenderDevice = nullptr;
	
	char buff[_MAX_PATH];
	snprintf(buff, sizeof(buff), "%s.dll", devicename);

	Msg("Loading DLL %s", buff);

	g_hRenderDeviceLib = LoadLibraryA(buff);
	if (g_hRenderDeviceLib != NULL)
	{
		createRenderDevice_t createRenderDeviceProc =
			(createRenderDevice_t)GetProcAddress(g_hRenderDeviceLib, "createRenderDevice");

		if (createRenderDeviceProc != NULL)
		{
			pRenderDevice = createRenderDeviceProc();
		}
	}

	ASSERT(pRenderDevice && "Failed to load render device lib");
	m_render_device = pRenderDevice;
}

void Engine::CreateSoundSystem(const char* soundname)
{
	ISoundSystem* pSoundSystem = NULL;
	char buff[_MAX_PATH]; snprintf(buff, sizeof(buff), "%s.dll", soundname);
	Msg("Loading DLL %s", buff);
	g_hSoundSystemLib = LoadLibrary(buff);
	if (g_hSoundSystemLib != NULL)
	{
		createSoundSystem_t createSoundSystemProc = (createSoundSystem_t)GetProcAddress(g_hSoundSystemLib, "createSoundSystem");
		if (createSoundSystemProc != NULL)
		{
			pSoundSystem = createSoundSystemProc();
		}
	}

	ASSERT(pSoundSystem && "Failed to load sound sys. Missing dll or OpenAL installation");
	g_pSoundSystem = pSoundSystem;
}

static gameLibShutdown_t s_gameLibShutdownPfn = NULL;

void Engine::CreateGameLib(const char* custompath)
{
	char buff[_MAX_PATH];
	snprintf(buff, sizeof(buff), "%s.dll", custompath);
	
	Msg("Loading DLL %s", buff);

	g_hGameLib = LoadLibraryA(buff);
	if (g_hGameLib != NULL)
	{
		gameLibInit_t gameLibInitPfn = 
			(gameLibInit_t)GetProcAddress(g_hGameLib, "gameLibInit");
		
		gameLibShutdown_t gameLibShutdownPfn = 
			(gameLibShutdown_t)GetProcAddress(g_hGameLib, "gameLibShutdown");

		if (gameLibInitPfn != NULL && gameLibShutdownPfn != NULL)
		{
			gameLibInitPfn();

			// make shutdown function as global variable
			s_gameLibShutdownPfn = gameLibShutdownPfn;
		}
	}

	ASSERT(s_gameLibShutdownPfn && "Missing exports from game library");
}

void Engine::CreateEditor()
{
	char buff[_MAX_PATH];
	snprintf(buff, sizeof(buff), "%s.dll", "editor");

	Msg("Loading DLL %s", buff);

	g_hEditorLib = LoadLibraryA(buff);
	if (g_hEditorLib != NULL)
	{
		createEditorSystem_t createEditorSystemProc = (createEditorSystem_t)GetProcAddress(g_hEditorLib, "createEditorSystem");
		if (createEditorSystemProc != NULL)
		{
			m_editor_system = createEditorSystemProc();
		}
	}

	ASSERT_MSG(m_editor_system, "Failed to create editor. Missing dll or exports?");

	m_editor_system->Init();
}

void Engine::Update()
{
	GetSystemTimer()->Update();

	float fDeltaTime = GetSystemTimer()->GetDelta();

	if (m_editor_system)
		m_editor_system->Update(fDeltaTime);
	
	g_pSoundSystem->Update(fDeltaTime);

	m_level->Update(fDeltaTime);

	//////////////////////////////////////////////////////////////////////////
	// Render Scene

	m_render_device->beginPass(m_viewport, PASSCLEAR_COLOR | PASSCLEAR_DEPTH);

	g_render.RenderScene();

	g_material_system.Render();

	m_render_device->endPass();
	m_render_device->commit();

	if (m_editor_system)
		m_editor_system->Render();

	m_render_device->present(false);
}

void Engine::Shutdown()
{
	if (m_editor_system) {
		m_editor_system->Shutdown();

		// destructor will clear g_pEditorSystem
		SAFE_DELETE(m_editor_system);

		FreeLibrary(g_hEditorLib);
	}

	if (g_pGamePersistent) {
		SAFE_DELETE(g_pGamePersistent);
	}

	s_gameLibShutdownPfn();
	FreeLibrary(g_hGameLib);

	g_render.Shutdown();
	
	if (m_render_device) {
		m_render_device->shutdown();
		SAFE_DELETE(m_render_device);
	}

	FreeLibrary(g_hRenderDeviceLib);

	if (m_level) {
		SAFE_DELETE(m_level);
	}

	if (g_object_factory) {
		SAFE_DELETE(g_object_factory);
	}

	if (g_input_system) {
		g_input_system->Shutdown();
		SAFE_DELETE(g_input_system);
	}

	if (g_pSoundSystem) {
		SAFE_DELETE(g_pSoundSystem);
	}

	FreeLibrary(g_hSoundSystemLib);

	if (m_render_window) {
		SDL_DestroyWindow(m_render_window);
		m_render_window = nullptr;
	}

	LogClose();

	IFileSystem::Destroy(g_file_system);

	SDL_Quit();
}

SDL_Window* Engine::GetRenderWindow() {
	return m_render_window;
}

IInputSystem* Engine::GetInputSystem() {
	return g_input_system;
}

Level* Engine::GetLevel()
{
	return m_level;
}

IRenderDevice* Engine::GetRenderDevice()
{
	return m_render_device;
}

IEditorSystem* Engine::GetEditorSystem()
{
	return m_editor_system;
}

viewport_t Engine::GetViewport()
{
	return m_viewport;
}

void Engine::OnWindowSizeChanged(uint32_t w, uint32_t h)
{
	m_viewport.width	= w;
	m_viewport.height	= h;
}
