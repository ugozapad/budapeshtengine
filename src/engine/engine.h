#ifndef ENGINE_H
#define ENGINE_H

#include "render/irenderdevice.h"

class IInputSystem;
class Level;

class ENGINE_API Engine {
public:
	Engine();
	~Engine();

	void init(int width, int height, bool fullscreen);
	void createRenderDevice(const char* devicename);
	void createSoundSystem(const char* soundname);
	void createGameLib(const char* custompath);

	void update();
	void shutdown();

	SDL_Window* getRenderWindow();
	IInputSystem* getInputSystem();
	Level* getLevel();
	IRenderDevice* getRenderDevice();

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

extern ENGINE_API Engine* g_engine;

#endif