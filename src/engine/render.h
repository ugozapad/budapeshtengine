#ifndef RENDER_H
#define RENDER_H

class Render
{
public:
	void Init();
	void Shutdown();

	void RenderScene();
};

extern Render g_render;

#endif // !RENDER_H
