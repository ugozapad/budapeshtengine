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

	void create(int width, int height, bool fullscreen);
	void createRenderDevice(const char* devicename);
	void createSoundSystem(const char* soundname);
	void createGameLib(const char* custompath);
	void createEditor();

	void update();
	void shutdown();

	SDL_Window* getRenderWindow();
	IInputSystem* getInputSystem();
	Level* getLevel();
	IRenderDevice* getRenderDevice();
	IEditorSystem* getEditorSystem();
	viewport_t getViewport();

	inline void requestExit		() { m_bExitRequested = true; }
	inline bool isExitRequested	() const { return (m_bExitRequested); }

	void		onWindowSizeChanged(uint32_t w, uint32_t h);

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