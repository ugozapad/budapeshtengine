#ifndef RENDER_H
#define RENDER_H

struct SDL_Window;

//! Interface to renderer
class IRender {
public:
	virtual ~IRender() {}

	virtual void init(SDL_Window* render_window) = 0;
	virtual void shutdown() = 0;

	virtual void renderFrame() = 0;
};

IRender* createRender();

#endif