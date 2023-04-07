#include "engine/allocator.h"
#include "render/render.h"

#ifdef NDEBUG
#undef NDEBUG
#endif // NDEBUG

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

	bufferIndex_t createBuffer(const bufferDesc_t& bufferDesc) override;
	void deleteBuffer(bufferIndex_t buffer_index) override;
	void updateBuffer(bufferIndex_t buffer_index, void* data, size_t size) override;

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

// #TODO: Rewrite to BufferResourceManager !!!

#define MAX_BUFFER 128

struct bufferResource_t {
	bufferIndex_t index;
	sg_buffer backend_buff;
};

static bufferResource_t s_buffer_resources[MAX_BUFFER];
static int s_buffer_resources_count = 0;

bufferIndex_t pushBuffer(sg_buffer buffer_backend) {
	bufferIndex_t buffer_index = s_buffer_resources_count;
	s_buffer_resources[s_buffer_resources_count++] = { buffer_index, buffer_backend };
	return buffer_index;
}

void popBuffer(bufferIndex_t index) {
	for (int i = 0; i < s_buffer_resources_count; i++) {
		bufferResource_t& buffer = s_buffer_resources[i];
		if (buffer.index == index) {
			buffer.index = INVALID_BUFFER_INDEX;
		}
	}
}

sg_buffer getBufferFromIndex(bufferIndex_t index) {
	for (int i = 0; i < s_buffer_resources_count; i++) {
		bufferResource_t& buffer = s_buffer_resources[i];
		if (buffer.index == index) {
			return buffer.backend_buff;
		}
	}

	return sg_buffer{0};
}

bufferIndex_t Render::createBuffer(const bufferDesc_t& buffer_desc) {
	// fill buffer data structure
	sg_range buffer_data = {0};
	buffer_data.ptr = buffer_desc.data;
	buffer_data.size = buffer_desc.size;

	// describe buffer
	sg_buffer_desc buffer_backend_desc = {0};
	if (buffer_desc.type == BUFFERTYPE_VERTEX) {
		buffer_backend_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
	} else if (buffer_desc.type == BUFFERTYPE_INDEX) {
		buffer_backend_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
	} else {
		// ERROR: sokol gfx does not support constant or uniform buffers at all sadly
		return INVALID_BUFFER_INDEX; //__debugbreak();
	}

	if (buffer_desc.access == BUFFERACCESS_STATIC) {
		buffer_backend_desc.usage = SG_USAGE_IMMUTABLE;
	} else if (buffer_desc.access == BUFFERACCESS_DYNAMIC) {
		buffer_backend_desc.usage = SG_USAGE_DYNAMIC;
	}

	/////////////////////////////////////////////////////////////////
	// #TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// if we want to create immutable buffer we should pass sg_range, 
	// otherwise set buffer_backend_desc.size from our buffer desc!
	/*if (buffer_desc.data) {
		buffer_backend_desc.data = buffer_data;
	} else {
		buffer_backend_desc.size = buffer_desc.size;
	}*/
	/////////////////////////////////////////////////////////////////
	buffer_backend_desc.data = buffer_data;
	buffer_backend_desc.size = buffer_desc.size;

	// create buffer
	sg_buffer buffer_backend = sg_make_buffer(buffer_backend_desc);
	sg_resource_state buffer_state = sg_query_buffer_state(buffer_backend);

	if (buffer_state == SG_RESOURCESTATE_INVALID) {
		// ERROR: No more free space in buffer pool
		return INVALID_BUFFER_INDEX;
	} else if (buffer_state == SG_RESOURCESTATE_FAILED) {
		// ERROR: failed to create buffer (reason is why should be prints in log)
		return INVALID_BUFFER_INDEX;
	}

	// push to resource man.
	return pushBuffer(buffer_backend);
}

void Render::deleteBuffer(bufferIndex_t buffer_index) {
	sg_buffer buffer_backend = getBufferFromIndex(buffer_index);
	if (sg_query_buffer_state(buffer_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: woops... we try to delete already destroyed buffer.
		__debugbreak(); // temp way
	}

	sg_destroy_buffer(buffer_backend);

	// remove from manager
	popBuffer(buffer_index);
}

void Render::updateBuffer(bufferIndex_t buffer_index, void* data, size_t size) {
	sg_buffer buffer_backend = getBufferFromIndex(buffer_index);
	if (sg_query_buffer_state(buffer_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: buffer invalid for some reason
		__debugbreak(); // temp way
	}
	
	sg_range data_range = {};
	data_range.ptr = data;
	data_range.size = size;
	sg_update_buffer(buffer_backend, data_range);
}
