#ifndef ENGINE_H
#define ENGINE_H

#include "render/irenderdevice.h"

class IInputSystem;
class IEditorSystem;
class Level;

class ENGINE_API Engine {
public:
	Engine();
	~Engine();

	void Create(int width, int height, bool fullscreen);
	void CreateRenderDevice(const char* devicename);
	void CreateSoundSystem(const char* soundname);
	void CreateGameLib(const char* custompath);
	void CreateEditor();

	void Update();
	void Shutdown();

	SDL_Window*			GetRenderWindow();
	IRenderDevice*		GetRenderDevice();
	IInputSystem*		GetInputSystem();
	IEditorSystem*		GetEditorSystem();
	Level*				GetLevel();
	viewport_t			GetViewport();

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
};

extern ENGINE_API Engine* g_engine;

#endif