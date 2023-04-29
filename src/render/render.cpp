#include "engine/debug.h"
#include "engine/allocator.h"
#include "render/render.h"

extern "C" {
#include "render/microui_render.h"
}

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
		m_default_clear_color_pass{0}
	,	m_default_clear_color_depth_pass{0}
	,	m_bindings{0}
	,	m_gl_context(nullptr)
	,	m_bindings_begin(false)
	{
		g_render = this;
	}

	~Render() {
		g_render = nullptr;
	}

	void init(SDL_Window* render_window) override;
	void shutdown() override;

	void uiFrame() override;
	void renderFrame() override;
	void draw(int base_element, int num_elements, int num_instances) override;

	bufferIndex_t createBuffer(const bufferDesc_t& buffer_desc) override;
	void deleteBuffer(bufferIndex_t buffer_index) override;
	void updateBuffer(bufferIndex_t buffer_index, void* data, size_t size) override;

	shaderIndex_t createShader(const shaderDesc_t& shader_desc) override;
	void deleteShader(shaderIndex_t shader) override;
	void setVSConstant(int ub_index, const void* data, size_t size) override;

	pipelineIndex_t createPipeline(const pipelineDesc_t& pipeline_desc) override;
	void deletePipeline(pipelineIndex_t pipeline) override;

	textureIndex_t createTexture(const textureDesc_t& texture_desc) override;
	void deleteTexture(textureIndex_t texture) override;

	void beginBinding() override;
	void setVertexBuffer(bufferIndex_t buffer_index) override;
	void setIndexBuffer(bufferIndex_t buffer_index) override;
	void setPipeline(pipelineIndex_t pipeline) override;
	void setTexture(int index, textureIndex_t texture) override;
	void endBinding() override;

	void beginPass(const viewport_t& viewport, passClearFlags_t pass_clear_flags) override;
	void endPass() override;
	void commit() override;
	void present(bool vsync) override;

private:
	SDL_Window* m_render_window;
	SDL_GLContext m_gl_context;
	sg_pass_action m_default_clear_color_pass;
	sg_pass_action m_default_clear_color_depth_pass;
	sg_bindings m_bindings;

	bool m_bindings_begin;
};

IRender* g_render = nullptr;

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

	// initialize passes
	m_default_clear_color_pass.colors[0].action = SG_ACTION_CLEAR;
	m_default_clear_color_pass.colors[0].value = { 0.5f, 0.5f, 0.5f, 1.0f };

	m_default_clear_color_depth_pass.colors[0].action = SG_ACTION_CLEAR;
	m_default_clear_color_depth_pass.colors[0].value = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_default_clear_color_depth_pass.depth.action = SG_ACTION_CLEAR;
	m_default_clear_color_depth_pass.depth.value = 1.0f;

	// initialize microui
	MicroUIRender_init();
}

void Render::shutdown() {
	// shutdown microui
	MicroUIRender_shutdown();

	// terminate sokol gfx
	sg_shutdown();

	// delete opengl context
	SDL_GL_MakeCurrent(m_render_window, nullptr);
	SDL_GL_DeleteContext(m_gl_context);
}

void Render::uiFrame() {
	int w = 0, h = 0;
	SDL_GetWindowSize(m_render_window, &w, &h);

	MicroUIRender_draw(w, h);
}

void Render::renderFrame() {


//	sg_begin_default_pass(&m_default_clear_pass, w, h);

		

	//sg_end_pass();
	//sg_commit();

	// swap buffer
//	SDL_GL_SwapWindow(m_render_window);
}

void Render::draw(int base_element, int num_elements, int num_instances) {
	sg_draw(base_element, num_elements, num_instances);
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
	if (buffer_desc.data) {
		buffer_backend_desc.data = buffer_data;
	} else {
		buffer_backend_desc.size = buffer_desc.size;
	}
	/////////////////////////////////////////////////////////////////
	//buffer_backend_desc.data = buffer_data;
	//buffer_backend_desc.size = buffer_desc.size;

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

#define MAX_SHADER 128

struct shaderResource_t {
	shaderIndex_t index;
	sg_shader backend_shd;
};

static shaderResource_t s_shader_resources[MAX_SHADER];
static int s_shader_resources_count = 0;

shaderIndex_t pushShader(sg_shader shader_backend) {
	shaderIndex_t shader_index = s_shader_resources_count;
	s_shader_resources[s_shader_resources_count++] = { shader_index, shader_backend };
	return shader_index;
}

void popShader(shaderIndex_t index) {
	for (int i = 0; i < s_shader_resources_count; i++) {
		shaderResource_t& shader = s_shader_resources[i];
		if (shader.index == index) {
			shader.index = INVALID_SHADER_INDEX;
		}
	}
}

sg_shader getShaderFromIndex(shaderIndex_t index) {
	for (int i = 0; i < s_shader_resources_count; i++) {
		shaderResource_t& shader = s_shader_resources[i];
		if (shader.index == index) {
			return shader.backend_shd;
		}
	}

	return sg_shader{ 0 };
}

void fillVSParams(sg_shader_desc& shader_desc) {
	sg_shader_uniform_block_desc& uniform_block1 = shader_desc.vs.uniform_blocks[0];
	uniform_block1.size = sizeof(float[4][4]);
	uniform_block1.uniforms[0].name = "u_model_matrix";
	uniform_block1.uniforms[0].type = SG_UNIFORMTYPE_MAT4;

	sg_shader_uniform_block_desc& uniform_block2 = shader_desc.vs.uniform_blocks[1];
	uniform_block2.size = sizeof(float[4][4]);
	uniform_block2.uniforms[0].name = "u_view_matrix";
	uniform_block2.uniforms[0].type = SG_UNIFORMTYPE_MAT4;

	sg_shader_uniform_block_desc& uniform_block3 = shader_desc.vs.uniform_blocks[2];
	uniform_block3.size = sizeof(float[4][4]);
	uniform_block3.uniforms[0].name = "u_proj_matrix";
	uniform_block3.uniforms[0].type = SG_UNIFORMTYPE_MAT4;

	sg_shader_uniform_block_desc& uniform_block4 = shader_desc.vs.uniform_blocks[3];
	uniform_block4.size = sizeof(float[4][4]);
	uniform_block4.uniforms[0].name = "u_model_view_projection";
	uniform_block4.uniforms[0].type = SG_UNIFORMTYPE_MAT4;
}

void fillFSParams(sg_shader_desc& shader_desc) {
	sg_shader_image_desc& image1 = shader_desc.fs.images[0];
	image1.image_type = SG_IMAGETYPE_2D;
	image1.name = "u_diffuse_texture";

	sg_shader_image_desc& image2 = shader_desc.fs.images[1];
	image2.image_type = SG_IMAGETYPE_2D;
	image2.name = "u_lightmap_texture";
}

shaderIndex_t Render::createShader(const shaderDesc_t& shader_desc) {
	sg_shader_desc shader_backend_desc = {0};
	shader_backend_desc.vs.source = shader_desc.vertex_shader_data;

	fillVSParams(shader_backend_desc);
	fillFSParams(shader_backend_desc);

	shader_backend_desc.fs.source = shader_desc.fragment_shader_data;

	sg_shader shader_backend = sg_make_shader(shader_backend_desc);
	if (sg_query_shader_state(shader_backend) != SG_RESOURCESTATE_VALID) {
		// Error: Failed to compile shader
		return INVALID_SHADER_INDEX;
	}

	return pushShader(shader_backend);
}

void Render::deleteShader(shaderIndex_t shader) {
	sg_shader shader_backend = getShaderFromIndex(shader);
	if (sg_query_shader_state(shader_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: woops... we try to delete already destroyed shader.
		__debugbreak(); // temp way
	}

	sg_destroy_shader(shader_backend);

	// remove from manager
	popShader(shader);
}

void Render::setVSConstant(int ub_index, const void* data, size_t size)
{
	sg_range vs_param_range = {};
	vs_param_range.ptr = data;
	vs_param_range.size = size;
	sg_apply_uniforms(SG_SHADERSTAGE_VS, ub_index, vs_param_range);
}

#define MAX_PIPELINE 128

struct pipelineResource_t {
	pipelineIndex_t index;
	sg_pipeline backend_pipe;
};

static pipelineResource_t s_pipeline_resources[MAX_PIPELINE];
static int s_pipeline_resources_count = 0;

pipelineIndex_t pushPipeline(sg_pipeline pipeline_backend) {
	pipelineIndex_t pipeline_index = s_pipeline_resources_count;
	s_pipeline_resources[s_pipeline_resources_count++] = { pipeline_index, pipeline_backend };
	return pipeline_index;
}

void popPipeline(pipelineIndex_t index) {
	for (int i = 0; i < s_pipeline_resources_count; i++) {
		pipelineResource_t& pipeline = s_pipeline_resources[i];
		if (pipeline.index == index) {
			pipeline.index = INVALID_PIPELINE_INDEX;
		}
	}
}

sg_pipeline getPipelineFromIndex(pipelineIndex_t index) {
	for (int i = 0; i < s_pipeline_resources_count; i++) {
		pipelineResource_t& pipeline = s_pipeline_resources[i];
		if (pipeline.index == index) {
			return pipeline.backend_pipe;
		}
	}

	return sg_pipeline{ 0 };
}

static const char* s_shader_semantic_gl_names[SHADERSEMANTIC_MAX] = {
	"position",
	"color",
	"texcoord",
	"texcoord0",
	"texcoord1",
	"normal",
	"tangent",
	"bitangent"
};

static const char* s_shader_semantic_d3d11_names[SHADERSEMANTIC_MAX] = {
	"POSITION",
	"COLOR",
	"TEXCOORD",
	"TEXCOORD0",
	"TEXCOORD1",
	"NORMAL",
	"TANGENT",
	"BITANGENT"
};

static sg_vertex_format s_vertex_formats_gl[VERTEXATTR_MAX] = {
	SG_VERTEXFORMAT_FLOAT2,
	SG_VERTEXFORMAT_FLOAT3,
	SG_VERTEXFORMAT_FLOAT4
};

pipelineIndex_t Render::createPipeline(const pipelineDesc_t& pipeline_desc) {
	sg_pipeline_desc pipeline_backend_desc = {};
	
	// force indices to uint16
	pipeline_backend_desc.index_type = SG_INDEXTYPE_UINT16;

	// get backend shader
	sg_shader shader_backend = getShaderFromIndex(pipeline_desc.shader);
	pipeline_backend_desc.shader = shader_backend;

	if (pipeline_desc.layout_count == 0) {
		printf("Render::createPipeline: ERROR: pipeline_desc.layout_count == 0\n");
		return INVALID_PIPELINE_INDEX;
	}

	// fill vertex attribute
	for (int i = 0; i < pipeline_desc.layout_count; i++) {
		const inputLayoutDesc_t input_layout_desc = pipeline_desc.layouts[i];
		sg_vertex_attr_desc& vertex_attrbite_desc = pipeline_backend_desc.layout.attrs[i];
		vertex_attrbite_desc.format = s_vertex_formats_gl[input_layout_desc.attribute];
	}

	sg_pipeline pipeline_backend = sg_make_pipeline(pipeline_backend_desc);
	if (sg_query_pipeline_state(pipeline_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: something wrong? see log
		return INVALID_PIPELINE_INDEX;
	}
	
	return pushPipeline(pipeline_backend);
}

void Render::deletePipeline(pipelineIndex_t pipeline) {
	sg_pipeline pipeline_backend = getPipelineFromIndex(pipeline);
	if (sg_query_pipeline_state(pipeline_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: woops... we try to delete already destroyed pipeline.
		__debugbreak(); // temp way
	}

	sg_destroy_pipeline(pipeline_backend);

	// remove from manager
	popPipeline(pipeline);
}

#define MAX_TEXTURE 128

struct imageResource_t {
	textureIndex_t index;
	sg_image backend_img;
};

static imageResource_t s_image_resources[MAX_TEXTURE];
static int s_image_resources_count = 0;

textureIndex_t pushImage(sg_image image_backend) {
	textureIndex_t image_index = s_image_resources_count;
	s_image_resources[s_image_resources_count++] = { image_index, image_backend };
	return image_index;
}

void popImage(textureIndex_t index) {
	for (int i = 0; i < s_image_resources_count; i++) {
		imageResource_t& image = s_image_resources[i];
		if (image.index == index) {
			image.index = INVALID_TEXTURE_INDEX;
		}
	}
}

sg_image getImageFromIndex(textureIndex_t index) {
	for (int i = 0; i < s_image_resources_count; i++) {
		imageResource_t& image = s_image_resources[i];
		if (image.index == index) {
			return image.backend_img;
		}
	}

	return sg_image{ 0 };
}

static sg_pixel_format s_sg_pixel_formats[TEXTUREFORMAT_MAX] = {
	SG_PIXELFORMAT_RGBA8,
	SG_PIXELFORMAT_RGBA8
};

textureIndex_t Render::createTexture(const textureDesc_t& texture_desc) {
	if (texture_desc.format >= TEXTUREFORMAT_MAX) {
		printf("Render::createTexture: ERROR: unknowed texture format!\n");
		__debugbreak();
	}

	if (texture_desc.type == TEXTURETYPE_1D) {
		printf("Render::createTexture: ERROR: TEXTURETYPE_1D is unsupported!\n");
		__debugbreak();
	}

	sg_image_desc image_backend_desc = {};
	image_backend_desc.pixel_format = s_sg_pixel_formats[texture_desc.format];
	image_backend_desc.width = texture_desc.width;
	image_backend_desc.height = texture_desc.height;
	image_backend_desc.type = (texture_desc.type == TEXTURETYPE_2D) ? SG_IMAGETYPE_2D : SG_IMAGETYPE_3D;
	image_backend_desc.usage = SG_USAGE_IMMUTABLE; 	// for now all textures is immutable
	image_backend_desc.min_filter = SG_FILTER_LINEAR;
	image_backend_desc.mag_filter = SG_FILTER_LINEAR;
	image_backend_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
	image_backend_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;

	// fill data
	image_backend_desc.data.subimage[0][0].ptr = texture_desc.data;
	image_backend_desc.data.subimage[0][0].size = texture_desc.size;

	sg_image image_backend = sg_make_image(image_backend_desc);

	if (sg_query_image_state(image_backend) != SG_RESOURCESTATE_VALID) {
		// Error: Failed to create image
		return INVALID_TEXTURE_INDEX;
	}

	return pushImage(image_backend);
}

void Render::deleteTexture(textureIndex_t texture) {
	sg_image image_backend = getImageFromIndex(texture);
	if (sg_query_image_state(image_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: woops... we try to delete already destroyed image.
		__debugbreak(); // temp way
	}

	sg_destroy_image(image_backend);

	// remove from manager
	popImage(texture);
}

void Render::beginBinding() {
	if (m_bindings_begin) {
		printf("Render::beginBinding: calling beginBinding without end previous bindings\n");
		__debugbreak();
	}

	m_bindings_begin = true;

	m_bindings = {};
}

void Render::setVertexBuffer(bufferIndex_t buffer_index) {
	sg_buffer buffer_backend = getBufferFromIndex(buffer_index);
	if (sg_query_buffer_state(buffer_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: buffer invalid for some reason
		__debugbreak(); // temp way
	}

	m_bindings.vertex_buffers[0] = buffer_backend;
}

void Render::setIndexBuffer(bufferIndex_t buffer_index) {
	sg_buffer buffer_backend = getBufferFromIndex(buffer_index);
	if (sg_query_buffer_state(buffer_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: buffer invalid for some reason
		__debugbreak(); // temp way
	}

	m_bindings.index_buffer = buffer_backend;
}

void Render::setPipeline(pipelineIndex_t pipeline) {
	sg_pipeline pipeline_backend = getPipelineFromIndex(pipeline);
	if (sg_query_pipeline_state(pipeline_backend) != SG_RESOURCESTATE_VALID) {
		// ERROR: pipeline invalid for some reason
		__debugbreak(); // temp way
	}

	sg_apply_pipeline(pipeline_backend);
}

void Render::setTexture(int index, textureIndex_t texture) {
	ASSERT(index <= SG_MAX_SHADERSTAGE_IMAGES);
	sg_image image_backend = getImageFromIndex(texture);
	m_bindings.fs_images[index] = image_backend;
}

void Render::endBinding() {
	if (!m_bindings_begin) {
		printf("Render::endBinding: calling endBinding without beginBindings\n");
		__debugbreak();
	}
	sg_apply_bindings(&m_bindings);
	m_bindings_begin = false;
}

void Render::beginPass(const viewport_t& viewport, passClearFlags_t pass_clear_flags) {
	sg_pass_action pass_action = {};

	if (pass_clear_flags & PASSCLEAR_COLOR) {
		pass_action.colors[0].action = SG_ACTION_CLEAR;
		pass_action.colors[0].value = { 0.5f, 0.5f, 0.5f, 1.0f };
	}
	if (pass_clear_flags & PASSCLEAR_DEPTH) {
		pass_action.depth.action = SG_ACTION_CLEAR;
		pass_action.depth.value = 1.0f;
	}

	sg_begin_default_pass(&pass_action, viewport.width, viewport.height);
}

void Render::endPass() {
	sg_end_pass();
}

void Render::commit() {
	sg_commit();
}

void Render::present(bool vsync) {
	SDL_GL_SetSwapInterval(vsync ? 1 : 0);
	SDL_GL_SwapWindow(m_render_window);
}
