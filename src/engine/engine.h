#ifndef ENGINE_H
#define ENGINE_H

#include "render/irenderdevice.h"

class IInputSystem;
class IEditorSystem;
class Level;

enum EngineState
{
	ENGINE_STATE_NEW_GAME,
	ENGINE_STATE_LOAD_GAME,
	ENGINE_STATE_CHANGE_LEVEL,
	ENGINE_STATE_RUNNING,
	ENGINE_STATE_QUIT,

	ENGINE_STATE_MAX
};

class ENGINE_API Engine {
public:
	Engine();
	~Engine();

	void Create(int width, int height, bool fullscreen);
	void CreateRenderDevice(const char* devicename);
	void CreateSoundSystem(const char* soundname);
	void CreateGameLib(const char* custompath);
	void CreateEditor();

	void SetState(EngineState state);
	void SetMapName(const char* mapname);

	void NewGame();

	void Update();
	void Shutdown();

	SDL_Window*			GetRenderWindow();
	IRenderDevice*		GetRenderDevice();
	IInputSystem*		GetInputSystem();
	IEditorSystem*		GetEditorSystem();
	Level*				GetLevel();
	viewport_t			GetViewport();
	EngineState			GetState();
	EngineState			GetNextState();

	inline void RequestExit		() { m_bExitRequested = true; }
	inline bool IsExitRequested	() const { return (m_bExitRequested); }

	void		OnWindowSizeChanged(uint32_t w, uint32_t h);

private:
	bool			m_bExitRequested;
	SDL_Window*		m_render_window;
	IRenderDevice*	m_render_device;
	IEditorSystem*	m_editor_system;
	Level*			m_level;
	viewport_t		m_viewport;

	const char*		m_map_name;

	// Engine state
	EngineState m_current_state;
	EngineState m_next_state;
};

ENGINE_API void Engine_NewGame(const char* map_name);

extern ENGINE_API Engine* g_engine;

#endif