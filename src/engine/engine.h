#ifndef ENGINE_H
#define ENGINE_H

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "render/irenderdevice.h"

class IInputSystem;
class Level;

class Engine {
public:
	Engine();
	~Engine();

	void init(int width, int height, bool fullscreen);
	void update();
	void shutdown();

	SDL_Window* getRenderWindow();
	IInputSystem* getInputSystem();
	Level* getLevel();

	inline void requestExit		() { m_bExitRequested = true; }
	inline bool isExitRequested	() const { return (m_bExitRequested); }

	void		onWindowSizeChanged(uint32_t w, uint32_t h);

private:
	bool			m_bExitRequested;
	SDL_Window*		m_render_window;
	IRenderDevice*	m_render_device;
	Level*			m_level;
	viewport_t		m_viewport;
};

extern Engine* g_engine;

#endif