#include "engine/level.h"
#include "engine/objectfactory.h"
#include "engine/filesystem.h"

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
	sprintf(levelpath, "data/levels/%s/", levelname);
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

	uint32_t lightmap_texture_len = 0;
	reader->read(&lightmap_texture_len, sizeof(lightmap_texture_len));
	char* lightmap_texture_path = (char*)g_default_allocator->allocate(size_t(lightmap_texture_len) + 1, 1);
	reader->read(lightmap_texture_path, lightmap_texture_len);

	lightmap_texture_path[lightmap_texture_len] = '\0';

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

	reader->read(&vertices[0], sizeof(LevelMeshVertex_LM) *vertices_count);

	//for (int i = 0; i < vertices_count; i++) {
	//	LevelMeshVertex_LM vertex;
	//	reader->read(&vertex, sizeof(vertex));
	//	vertices.push_back(vertex);
	//}
	
	uint32_t indices_count = 0;
	reader->read(&indices_count, sizeof(indices_count));

	// #TODO: ???
	//reader->seek(SeekWay::Current, indices_count);

	// create gpu resource
	createGpu(vertices);
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

		lightmapped_generic_shader = g_render->createShader(shader_desc);
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
		pipeline_desc.layout_count = sizeof(pipeline_desc.layouts) / sizeof(pipeline_desc.layouts[0]);

		lightmapped_generic_pipe = g_render->createPipeline(pipeline_desc);
		if (lightmapped_generic_pipe == INVALID_PIPELINE_INDEX) {
			printf("!!! lightmapped_generic_pipe == INVALID_PIPELINE_INDEX\n");
			DebugBreak();
		}

		is_lightmapped_shader_inited = true;
	}

	g_render->setPipeline(lightmapped_generic_pipe);
}

void LevelMesh::render() {
	bindLightmapShader();

	g_render->beginBinding();
		g_render->setVertexBuffer(m_vertex_buffer);
	g_render->endBinding();

	g_render->draw(m_vertices_count, 0, 0);
}

void LevelMesh::createGpu(Array<LevelMeshVertex_LM>& vertices) {
	bufferDesc_t vertex_buffer_desc = {};
	vertex_buffer_desc.type = BUFFERTYPE_VERTEX;
	vertex_buffer_desc.access = BUFFERACCESS_STATIC;
	vertex_buffer_desc.data = &vertices[0];
	vertex_buffer_desc.size = vertices.size();
	m_vertex_buffer = g_render->createBuffer(vertex_buffer_desc);
}
