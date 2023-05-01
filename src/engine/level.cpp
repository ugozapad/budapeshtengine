#include "engine/level.h"
#include "engine/objectfactory.h"
#include "engine/filesystem.h"
#include "engine/engine.h"
#include "engine/camera.h"

#include "render/texture.h"

#include <glm/glm.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

Level::Level(IAllocator& allocator) :
	m_allocator(&allocator),
	m_entities(allocator)
{
}

Level::~Level()
{
}

void Level::load(const char* levelname) {
	IReader* reader;
	char levelpath[512];
	snprintf(levelpath, 512, "data/levels/%s/", levelname);
	size_t const pathend = strlen(levelpath);
	
	strcpy(levelpath + pathend, "level.lmf");
	reader = g_file_system->openRead(levelpath);
	loadLMF(reader);
	g_file_system->deleteReader(reader);
	
	// strcpy(levelpath + pathend, "level.somefile");
	// reader = g_file_system->openRead(levelpath);
	// loadSomeFile(reader);
	// etc.
}

void Level::loadLMF(IReader* reader) {
	// read header
	LevelMeshHeader header;
	reader->read(&header, sizeof(header));

	for (uint16_t i = 0; i < header.mesh_count; i++) {
		LevelMesh* level_mesh = MEM_NEW(*m_allocator, LevelMesh);
		level_mesh->load(reader);

		Entity* level_mesh_entity = level_mesh;
		m_entities.push_back(level_mesh_entity);
	}
}

Entity* Level::createEntity() {
	Entity* entity = MEM_NEW(*m_allocator, Entity);
	m_entities.push_back(entity);
	return entity;
}

void Level::addEntity(Entity* entity) {
	if (!entity) {
		printf("Level::addEntity: failed to add null ptr entity\n");
		DebugBreak();
	}

	size_t entity_count = m_entities.size();
	for (size_t i = 0; i < entity_count; i++) {
		if (m_entities[i] == entity) {
			printf("Level::addEntity: entity (classname=%s ptr=0x%p) already exists\n", entity->getClassName(), entity);
			DebugBreak();
		}
	}

	m_entities.push_back(entity);
}

void Level::render() {
	for (Array<Entity*>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
		Entity* entity = (*it);
		if (LevelMesh* level_mesh = dynamicCast<LevelMesh>(entity)) {
			level_mesh->render();
		}

	}
}

///////////////////////////////////////////////////////////////////
// Level mesh

LevelMesh::LevelMesh() :
	m_vertex_buffer(INVALID_BUFFER_INDEX),
	m_index_buffer(INVALID_BUFFER_INDEX),
	m_vertices_count(0),
	m_mesh_name(nullptr)
{
}

LevelMesh::~LevelMesh() {
}

void LevelMesh::load(IReader* reader) {

	// Load LevelMeshMaterial
	uint32_t diffuse_texture_len = 0;
	reader->read(&diffuse_texture_len, sizeof(diffuse_texture_len));
	char* diffuse_texture_path = (char*)g_default_allocator->allocate(size_t(diffuse_texture_len) + 1, 1);
	reader->read(diffuse_texture_path, diffuse_texture_len);

	diffuse_texture_path[diffuse_texture_len] = '\0';

	for (int i = 0; i < diffuse_texture_len; i++) {
		if (diffuse_texture_path[i] == '\\')
			diffuse_texture_path[i] = '/';
	}

	// load texture
	m_diffuse_texture = MEM_NEW(*g_default_allocator, Texture, *g_default_allocator, *g_render_device);
	
	char* path_to_texture = strstr(diffuse_texture_path, "/temp/") + strlen("/temp/");

	char buffer1[MAX_PATH];
	snprintf(buffer1, sizeof(buffer1), "data/textures/temp/%s", path_to_texture);
	
	IReader* texture_reader = g_file_system->openRead(buffer1);
	m_diffuse_texture->load(texture_reader, true);

	g_file_system->deleteReader(texture_reader);

	uint32_t lightmap_texture_len = 0;
	reader->read(&lightmap_texture_len, sizeof(lightmap_texture_len));
	char* lightmap_texture_path = (char*)g_default_allocator->allocate(size_t(lightmap_texture_len) + 1, 1);
	reader->read(lightmap_texture_path, lightmap_texture_len);

	lightmap_texture_path[lightmap_texture_len] = '\0';

	for (int i = 0; i < lightmap_texture_len; i++) {
		if (lightmap_texture_path[i] == '\\')
			lightmap_texture_path[i] = '/';
	}

	char buffer2[MAX_PATH];
	snprintf(buffer2, sizeof(buffer2), "data/levels/%s/%s", "test_baking", lightmap_texture_path);

	m_lightmap_texture = MEM_NEW(*g_default_allocator, Texture, *g_default_allocator, *g_render_device);

	texture_reader = g_file_system->openRead(buffer2);
	m_lightmap_texture->load(texture_reader, false);

	g_file_system->deleteReader(texture_reader);

	g_default_allocator->deallocate(lightmap_texture_path);
	g_default_allocator->deallocate(diffuse_texture_path);

	// Load LevelMesh
	uint32_t mesh_name_len = 0;
	reader->read(&mesh_name_len, sizeof(mesh_name_len));
	char* mesh_name = (char*)g_default_allocator->allocate(size_t(mesh_name_len) + 1, 1);
	reader->read(mesh_name, mesh_name_len);

	mesh_name[mesh_name_len] = '\0';

	g_default_allocator->deallocate(mesh_name);

	uint32_t vertices_count = 0;
	reader->read(&vertices_count, sizeof(vertices_count));

	m_vertices_count = vertices_count;

	Array<LevelMeshVertex_LM> vertices(*g_default_allocator);
	vertices.resize(vertices_count);

	reader->read(vertices.data(), sizeof(LevelMeshVertex_LM) * vertices_count);

	int counter = 0;
	for (Array<LevelMeshVertex_LM>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
		LevelMeshVertex_LM& vertex = (*it);
		//printf("%i:\t%f\t%f\n", counter, vertex.texcoord1.x, vertex.texcoord1.y);
		counter++;
	}
	
	uint32_t indices_count = 0;
	reader->read(&indices_count, sizeof(indices_count));

	m_indices_count = indices_count;

	Array<uint16_t> indices(*g_default_allocator);
	indices.resize(indices_count);

	reader->read(indices.data(), indices_count * sizeof(uint16_t));

	// #TODO: ???
	//reader->seek(SeekWay::Current, indices_count);

	// create gpu resource
	createGpu_Vertex(vertices);
	createGpu_Indices(indices);
}

void bindLightmapShader() {
	static bool is_lightmapped_shader_inited = false;
	static shaderIndex_t lightmapped_generic_shader = INVALID_SHADER_INDEX;
	static pipelineIndex_t lightmapped_generic_pipe = INVALID_PIPELINE_INDEX;
	if (!is_lightmapped_shader_inited) {
		IReader* reader = g_file_system->openRead("data/shaders/gl33/lightmapped_generic.vs");
		reader->seek(End, 0);
		size_t vertex_length = reader->tell();
		reader->seek(Begin, 0);

		char* vertex_shader = (char*)g_default_allocator->allocate(vertex_length + 1, 1);
		reader->read(vertex_shader, vertex_length);
		vertex_shader[vertex_length] = '\0';

		g_file_system->deleteReader(reader);
		
		// reopen reader for fragment shader
		reader = g_file_system->openRead("data/shaders/gl33/lightmapped_generic.fs");
		reader->seek(End, 0);
		size_t fragment_length = reader->tell();
		reader->seek(Begin, 0);

		char* fragment_shader = (char*)g_default_allocator->allocate(fragment_length + 1, 1);
		reader->read(fragment_shader, fragment_length);
		fragment_shader[fragment_length] = '\0';

		g_file_system->deleteReader(reader);

		shaderDesc_t shader_desc = {};
		shader_desc.vertex_shader_data = vertex_shader;
		shader_desc.vertex_shader_size = vertex_length;
		shader_desc.fragment_shader_data = fragment_shader;
		shader_desc.fragment_shader_size = fragment_length;

		lightmapped_generic_shader = g_render_device->createShader(shader_desc);
		if (lightmapped_generic_shader == INVALID_SHADER_INDEX) {
			printf("!!! lightmapped_generic_shader == INVALID_SHADER_INDEX\n");
			DebugBreak();
		}

		// free data
		g_default_allocator->deallocate(fragment_shader);
		g_default_allocator->deallocate(vertex_shader);

		pipelineDesc_t pipeline_desc = {};
		pipeline_desc.shader = lightmapped_generic_shader;
		pipeline_desc.layouts[0] = { VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION };
		pipeline_desc.layouts[1] = { VERTEXATTR_VEC2, SHADERSEMANTIC_TEXCOORD0 };
		pipeline_desc.layouts[2] = { VERTEXATTR_VEC2, SHADERSEMANTIC_TEXCOORD1 };
		pipeline_desc.layout_count = 3;

		lightmapped_generic_pipe = g_render_device->createPipeline(pipeline_desc);
		if (lightmapped_generic_pipe == INVALID_PIPELINE_INDEX) {
			printf("!!! lightmapped_generic_pipe == INVALID_PIPELINE_INDEX\n");
			DebugBreak();
		}

		is_lightmapped_shader_inited = true;
	}

	g_render_device->setPipeline(lightmapped_generic_pipe);
}

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void LevelMesh::render() {
	bindLightmapShader();

	glm::mat4 s_mat4_idenitity = glm::mat4(1.0f);

	glm::vec3 cameraPos = glm::vec3(5.0f, 7.0f, 5.0f);
	g_camera.setPosition(cameraPos);

	// calculate aspect ratio
	int width = 0, height = 0;
	SDL_GetWindowSize(g_engine->getRenderWindow(), &width, &height);

	float aspectRatio = (float)width / (float)height;

	// calculate projection matrix
	glm::mat4 proj = s_mat4_idenitity;
	proj = glm::perspective(glm::radians(75.0f), aspectRatio, 0.1f, 1000.0f);

	// calculate model view projection matrix
	glm::mat4 mvp = s_mat4_idenitity;
	mvp = proj * g_camera.getViewMatrix() * s_mat4_idenitity;

	g_render_device->setVSConstant(CONSTANT_MODEL_MATRIX, &s_mat4_idenitity[0], MATRIX4_SIZE);
	g_render_device->setVSConstant(CONSTANT_VIEW_MATRIX, &s_mat4_idenitity[0], MATRIX4_SIZE);
	g_render_device->setVSConstant(CONSTANT_PROJ_MATRIX, &s_mat4_idenitity[0], MATRIX4_SIZE);

	g_render_device->setVSConstant(CONSTANT_MVP_MATRIX, &mvp[0], MATRIX4_SIZE);

	g_render_device->beginBinding();
		g_render_device->setTexture(0, m_diffuse_texture->getTextureIndex());
		g_render_device->setTexture(1, m_lightmap_texture->getTextureIndex());
		g_render_device->setVertexBuffer(m_vertex_buffer);
		g_render_device->setIndexBuffer(m_index_buffer);
	g_render_device->endBinding();

	g_render_device->draw(0, m_indices_count, 1);
}

void LevelMesh::createGpu_Vertex(Array<LevelMeshVertex_LM>& vertices) {
	bufferDesc_t vertex_buffer_desc = {};
	vertex_buffer_desc.type = BUFFERTYPE_VERTEX;
	vertex_buffer_desc.access = BUFFERACCESS_STATIC;
	vertex_buffer_desc.data = &vertices[0];
	vertex_buffer_desc.size = vertices.size() * sizeof(LevelMeshVertex_LM);
	m_vertex_buffer = g_render_device->createBuffer(vertex_buffer_desc);
}

void LevelMesh::createGpu_Indices(Array<uint16_t>& indices) {
	bufferDesc_t index_buffer_desc = {};
	index_buffer_desc.type = BUFFERTYPE_INDEX;
	index_buffer_desc.access = BUFFERACCESS_STATIC;
	index_buffer_desc.data = &indices[0];
	index_buffer_desc.size = indices.size() * sizeof(uint16_t);
	m_index_buffer = g_render_device->createBuffer(index_buffer_desc);
}
