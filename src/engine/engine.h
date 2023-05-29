#ifndef ENGINE_H
#define ENGINE_H

#define SDL_MAIN_HANDLED
#include "SDL.h"

class IInputSystem;
class Level;

class Engine {
public:
	Engine();
	~Engine();

	void init(int width, int height, bool fullscreen);
	void shutdown();

	SDL_Window* getRenderWindow();
	IInputSystem* getInputSystem();
	Level* getLevel();

	inline void requestExit		() { m_bExitRequested = true; }
	inline bool isExitRequested	() const { return (m_bExitRequested); }

private:
	bool		m_bExitRequested;
	SDL_Window* m_render_window;
	Level* m_level;
};

extern Engine* g_engine;

#endif