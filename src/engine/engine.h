#ifndef ENGINE_H
#define ENGINE_H

#define SDL_MAIN_HANDLED
#include "SDL.h"

class IInputSystem;
class IRenderDevice;
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

	IRenderDevice* getRenderDevice();

private:
	SDL_Window* m_render_window;
	Level* m_level;
	IRenderDevice* m_render_device;
};

#endif