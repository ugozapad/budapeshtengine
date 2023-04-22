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

void Level::load(const char* filepath) {
	if (strstr(filepath, ".lmf")) {
		return loadLMF(filepath);
	}
}

void Level::loadLMF(const char* filepath) {
	// open reader
	IReader* reader = g_file_system->openRead(filepath);
	
	// read header
	LevelMeshHeader header;
	reader->read(&header, sizeof(header));

	for (int i = 0; i < header.mesh_count; i++) {
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

	Array<LevelMeshVertex_LM> vertices(*g_default_allocator);

	LevelMeshVertex_LM vertex;
	for (uint32_t i = 0; i < vertices_count; i++) {
		reader->read(&vertex, sizeof(vertex));
		vertices.push_back(vertex);
	}

	uint32_t indices_count = 0;
	reader->read(&indices_count, sizeof(indices_count));

	// #TODO: ???
	//reader->seek(SeekWay::Current, indices_count);

	// create gpu resource
//	createGpu(vertices);
}

void bindLightmapShader() {

}

void LevelMesh::render() {
	bindLightmapShader();

	g_render->beginBinding();
		g_render->setVertexBuffer( m_vertex_buffer );
	g_render->endBinding();

	g_render->draw(1337, 0, 0);
}

void LevelMesh::createGpu(Array<LevelMeshVertex_LM>& vertices) {
	bufferDesc_t vertex_buffer_desc = {};
	vertex_buffer_desc.type = BUFFERTYPE_VERTEX;
	vertex_buffer_desc.access = BUFFERACCESS_STATIC;
	vertex_buffer_desc.data = &vertices[0];
	vertex_buffer_desc.size = vertices.size();
	m_vertex_buffer = g_render->createBuffer(vertex_buffer_desc);
}
