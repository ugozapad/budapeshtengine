#ifndef ENGINE_H
#define ENGINE_H

#define SDL_MAIN_HANDLED
#include "SDL.h"

class Engine {
public:
	Engine();
	~Engine();

	void init(int width, int height, bool fullscreen);
	void shutdown();

	SDL_Window* getRenderWindow();

private:
	SDL_Window* m_render_window;
};

#endif