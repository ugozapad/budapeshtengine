#include "pch.h"
#include "engine/allocator.h"
#include "engine/filesystem.h"
#include "engine/engine.h"
#include "engine/shader_engine.h"
#include "engine/level_mesh.h"
#include "engine/camera.h"
#include "engine/texture.h"
#include "engine/material_system.h"
#include "engine/logger.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

///////////////////////////////////////////////////////////////////
// Level mesh

LevelMesh::LevelMesh() :
	m_mesh_name(nullptr),
	m_mesh(nullptr)
{
}

LevelMesh::~LevelMesh() {

	if (m_mesh) {
		SAFE_DELETE(m_mesh);
	}

}

void LevelMesh::load(IReader* reader) {

	// Load LevelMeshMaterial
	uint32_t diffuse_texture_len = 0;
	reader->read(&diffuse_texture_len, sizeof(diffuse_texture_len));
	char* diffuse_texture_path = mem_tcalloc<char>(diffuse_texture_len + 1);
	reader->read(diffuse_texture_path, diffuse_texture_len);

	diffuse_texture_path[diffuse_texture_len] = '\0';

	for (uint32_t i = 0; i < diffuse_texture_len; i++) {
		if (diffuse_texture_path[i] == '\\')
			diffuse_texture_path[i] = '/';
	}

	char* path_to_texture = strstr(diffuse_texture_path, "/temp/") + strlen("/temp/");

	char buffer1[_MAX_PATH];
	snprintf(buffer1, sizeof(buffer1), "data/textures/temp/%s", path_to_texture);

	uint32_t lightmap_texture_len = 0;
	reader->read(&lightmap_texture_len, sizeof(lightmap_texture_len));
	char* lightmap_texture_path = mem_tcalloc<char>(lightmap_texture_len + 1);
	reader->read(lightmap_texture_path, lightmap_texture_len);

	lightmap_texture_path[lightmap_texture_len] = '\0';

	for (uint32_t i = 0; i < lightmap_texture_len; i++) {
		if (lightmap_texture_path[i] == '\\')
			lightmap_texture_path[i] = '/';
	}

	char buffer2[260];
	snprintf(buffer2, sizeof(buffer2), "data/levels/%s/%s", "test_baking", lightmap_texture_path);

	// Load LevelMesh
	uint32_t mesh_name_len = 0;
	reader->read(&mesh_name_len, sizeof(mesh_name_len));
	char* mesh_name = mem_tcalloc<char>(mesh_name_len + 1);
	reader->read(mesh_name, mesh_name_len);

	mesh_name[mesh_name_len] = '\0';

	mem_free(mesh_name);

	uint32_t vertices_count = 0;
	reader->read(&vertices_count, sizeof(vertices_count));

	Array<LevelMeshVertex_LM> vertices;
	vertices.resize(vertices_count);

	reader->read(vertices.data(), sizeof(LevelMeshVertex_LM) * vertices_count);

	int counter = 0;
	for (Array<LevelMeshVertex_LM>::iterator it = vertices.begin(); it != vertices.end(); ++it)
	{
		LevelMeshVertex_LM& vertex = (*it);
		if (isnan(vertex.position.x) || isnan(vertex.position.y) || isnan(vertex.position.z))
		{
			Msg("triangle %i reset to zero size", counter);
			vertex.position.x = 0.0f;
			vertex.position.y = 0.0f;
			vertex.position.z = 0.0f;
		}

		counter++;
	}

	uint32_t indices_count = 0;
	reader->read(&indices_count, sizeof(indices_count));

	Array<uint16_t> indices;
	indices.resize(indices_count);

	reader->read(indices.data(), indices_count * sizeof(uint16_t));

	// Create mesh object
	m_mesh = new StaticLevelMesh(vertices,
		indices,
		"lightmapped_generic",
		buffer1,
		buffer2);

	mem_free(lightmap_texture_path);
	mem_free(diffuse_texture_path);
}

static glm::mat4 s_mat4_idenitity = glm::mat4(1.0f);

void LevelMesh::render(const renderContext_t& render_context) {
	m_mesh->draw(getTranslationMatrix(), render_context);
}

//////////////////////////////////////////////////////////////////////////////////////

DynamicMeshEntity::DynamicMeshEntity()
{
}

DynamicMeshEntity::~DynamicMeshEntity()
{
	for (auto it : m_meshes) {
		delete(it);
	}
}

void DynamicMeshEntity::loadModel(const char* filename)
{
	IReader* reader = g_file_system->openRead(filename);
	ASSERT(reader);

	// read header
	LevelMeshHeader header;
	reader->read(&header, sizeof(header));

	for (uint16_t i = 0; i < header.mesh_count; i++)
	{
		DynamicMesh* dynamic_mesh = DynamicMesh::createFromStream(reader);
		m_meshes.push_back(dynamic_mesh);
	}
}

void DynamicMeshEntity::render(const renderContext_t& render_context)
{
	for (auto it : m_meshes) {
		it->draw(getTranslationMatrix(), render_context);
	}
}
