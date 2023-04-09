#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "engine/allocator.h"
#include "engine/filesystem.h"
#include "engine/engine.h"
#include "render/render.h"
#include "render/texture.h"

class Main {
public:
	Main() :
		m_engine(nullptr)
	,	m_render(nullptr)
	{
	}

	int init(int argc, char* argv[]);
	void shutdown();

	void update();

private:
	Engine* m_engine;
	IRender* m_render;
};

static Main s_main;
static HANDLE s_locker_mutex;
static bufferIndex_t s_buffer;
static shaderIndex_t s_shader;
static pipelineIndex_t s_pipeline;
static Texture* s_texture;

int Main::init(int argc, char* argv[]) {

	bool createLockerMutex = true;
	bool fullscreen = false;

	for (int i = 0; i < argc; i++) {
		if (strcmp("-multinstance", argv[i]) == 0)
			createLockerMutex = false;
		if (strcmp("-fullscreen", argv[0]) == 0)
			fullscreen = true;
	}

	if (createLockerMutex) {
		s_locker_mutex = OpenMutexA(READ_CONTROL, FALSE, "execution_locker_mutex");
		if (s_locker_mutex) {
			MessageBoxA(nullptr, "Failed to open second instance.", "Error", MB_OK | MB_ICONERROR);
			return 2;
		} else {
			s_locker_mutex = CreateMutexA(NULL, FALSE, "execution_locker_mutex");
		}
	}

	g_default_allocator = createDefaultAllocator();

	m_engine = MEM_NEW(*g_default_allocator, Engine);
	m_engine->init(1024, 768, fullscreen);

	m_render = createRender();
	m_render->init(m_engine->getRenderWindow());

	float vertices[] = {
		// positions            // colors
		 0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
	};

	bufferDesc_t buffer_desc = {};
	buffer_desc.type = BUFFERTYPE_VERTEX;
	buffer_desc.access = BUFFERACCESS_STATIC;
	buffer_desc.data = vertices;
	buffer_desc.size = sizeof(vertices);
	s_buffer = m_render->createBuffer(buffer_desc);
	if (s_buffer == INVALID_BUFFER_INDEX)
		__debugbreak();

	shaderDesc_t shader_desc = {};
	shader_desc.vertex_shader_data = "#version 330\n"
		"layout(location=0) in vec4 position;\n"
		"layout(location=1) in vec4 color0;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"  gl_Position = position;\n"
		"  color = color0;\n"
		"}\n";

	shader_desc.vertex_shader_size = strlen(shader_desc.vertex_shader_data) + 1;

	shader_desc.fragment_shader_data = "#version 330\n"
		"in vec4 color;\n"
		"out vec4 frag_color;\n"
		"void main() {\n"
		"  frag_color = color;\n"
		"}\n";

	shader_desc.fragment_shader_size = strlen(shader_desc.fragment_shader_data) + 1;

	s_shader = m_render->createShader(shader_desc);
	if (s_shader == INVALID_SHADER_INDEX)
		__debugbreak();

	pipelineDesc_t pipeline_desc = {};
	pipeline_desc.layouts[0] = { VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION };
	pipeline_desc.layouts[1] = { VERTEXATTR_VEC4, SHADERSEMANTIC_COLOR };
	pipeline_desc.layout_count = 2;
	pipeline_desc.shader = s_shader;

	s_pipeline = m_render->createPipeline(pipeline_desc);
	if (s_pipeline == INVALID_PIPELINE_INDEX)
		__debugbreak();

	// test stuff
	s_texture = MEM_NEW(*g_default_allocator, Texture, *g_default_allocator, *m_render);

	IReader* texture_reader = g_file_system->openRead("data/textures/test_image.bmp");
	s_texture->load(texture_reader);
	g_file_system->deleteReader(texture_reader);

	return 0;
}

void Main::shutdown() {
	MEM_DELETE(*g_default_allocator, Texture, s_texture);

	m_render->deleteBuffer(s_buffer);

	m_render->shutdown();
	MEM_DELETE(*g_default_allocator, IRender, m_render);

	m_engine->shutdown();
	MEM_DELETE(*g_default_allocator, Engine, m_engine);
}

void Main::update()
{
	viewport_t viewport = { 0,0,1024,768 };
	m_render->beginPass(viewport, PASSCLEAR_COLOR);

	m_render->setPipeline(s_pipeline);
	m_render->setVertexBuffer(s_buffer);
	//m_render->setTexture(s_texture->getTextureIndex());

	m_render->draw(0, 3, 1);

	m_render->endPass();
	m_render->commit();
	m_render->present(false);
}

int main(int argc, char* argv[]) {

	int retval = s_main.init(argc, argv);

	bool run = true;
	while (run) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				run = false;
			}
		}

		s_main.update();
	}

	s_main.shutdown();

	return 0;
}

