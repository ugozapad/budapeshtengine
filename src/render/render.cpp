#include "engine/allocator.h"
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

	void updateBuffer(uint32_t buffer_index, void* data, size_t size) override;

private:
	SDL_Window* m_render_window;
	SDL_GLContext m_gl_context;
	sg_pass_action m_default_clear_pass;
};

IRender* createRender() {
	return MEM_NEW(*g_default_allocator, Render);
}

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
	int w = 0, h = 0;
	SDL_GetWindowSize(m_render_window, &w, &h);

	sg_begin_default_pass(&m_default_clear_pass, w, h);
	sg_end_pass();
	sg_commit();

	// swap buffer
	SDL_GL_SwapWindow(m_render_window);
}

sg_buffer getBufferFromIndex(uint32_t index) {
	return sg_buffer{};
}

void Render::updateBuffer(uint32_t buffer_index, void* data, size_t size) {
	sg_range data_range = {};
	data_range.ptr = data;
	data_range.size = size;
	sg_update_buffer(getBufferFromIndex(buffer_index), data_range);
}
