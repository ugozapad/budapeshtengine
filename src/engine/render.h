#ifndef RENDER_H
#define RENDER_H

class Render
{
public:
	void init();
	void shutdown();

	void renderScene();
};

extern Render g_render;

#endif // !RENDER_H
