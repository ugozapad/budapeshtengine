#include "render/render.h"

#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE33
#include "sokol_gfx.h"

#define SOKOL_LOG_IMPL
#include "sokol_log.h"

class Render : public IRender {
public:
	Render() :
		m_default_clear_pass{0}
	{}
	~Render() {}

	void init() override;
	void shutdown() override;

private:
	sg_pass_action m_default_clear_pass;
};

void Render::init() {
	// initialize sokol-gfx
	sg_desc desc = {};
	desc.logger.func = slog_func;
	sg_setup(desc);

	// initialize pass
	m_default_clear_pass.colors[0].action = SG_ACTION_CLEAR;
	m_default_clear_pass.colors[0].value = { 0.5f, 0.5f, 0.5f, 1.0f };
}

void Render::shutdown() {
	// terminate sokol-gfx
	sg_shutdown();
}

IRender* createRender()
{
	return new Render();
}
