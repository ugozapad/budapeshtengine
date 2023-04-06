#include "render/render.h"

#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE33
#include "sokol_gfx.h"

#define SOKOL_LOG_IMPL
#include "sokol_log.h"

#include "SDL.h"

class Render : public IRender {
public:
	Render() :
		m_default_clear_pass{0}
	,	m_gl_context(nullptr)
	{}
	~Render() {}

	void init(SDL_Window* render_window) override;
	void shutdown() override;

	void renderFrame() override;

private:
	SDL_Window* m_render_window;
	SDL_GLContext m_gl_context;
	sg_pass_action m_default_clear_pass;
};

void Render::init(SDL_Window* render_window) {
	m_render_window = render_window;

	// create render context
	m_gl_context = SDL_GL_CreateContext(m_render_window);
	SDL_GL_MakeCurrent(m_render_window, m_gl_context);

	// initialize sokol gfx
	sg_desc desc = {};
	desc.logger.func = slog_func;
	sg_setup(desc);

	// initialize pass
	m_default_clear_pass.colors[0].action = SG_ACTION_CLEAR;
	m_default_clear_pass.colors[0].value = { 0.5f, 0.5f, 0.5f, 1.0f };
}

void Render::shutdown() {
	// terminate sokol gfx
	sg_shutdown();

	// delete opengl context
	SDL_GL_MakeCurrent(m_render_window, nullptr);
	SDL_GL_DeleteContext(m_gl_context);
}

void Render::renderFrame() {
	sg_begin_default_pass(&m_default_clear_pass, 1024, 768);
	sg_end_pass();
	sg_commit();

	// swap buffer
	SDL_GL_SwapWindow(m_render_window);
}

IRender* createRender()
{
	return new Render();
}
