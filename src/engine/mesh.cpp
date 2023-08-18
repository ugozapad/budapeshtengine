#include "pch.h"
#include "engine/mesh.h"
#include "engine/engine.h"
#include "engine/shader_engine.h"
#include "engine/texture.h"
#include "engine/material_system.h"
#include "engine/filesystem.h"

StaticLevelMesh::StaticLevelMesh(Array<LevelMeshVertex_LM>& vertices, 
	Array<uint16_t>& indices, 
	const char* material_name,
	const char* texture_name, 
	const char* lm_name) :
	m_vertex_buffer(INVALID_BUFFER_INDEX),
	m_index_buffer(INVALID_BUFFER_INDEX),
	m_pipeline(INVALID_PIPELINE_INDEX),
	m_diffuse_texture(nullptr),
	m_lightmap_texture(nullptr)
{
	ASSERT(!vertices.empty());
	ASSERT(!indices.empty());

	m_vertices_count = uint32_t(vertices.size());
	m_indices_count = uint32_t(indices.size());

	CreateGpu_Vertex(vertices);
	CreateGpu_Indices(indices);
	CreateGpu_Material(material_name, texture_name, lm_name);
}

StaticLevelMesh::~StaticLevelMesh()
{
	DestroyGpu();
}

void StaticLevelMesh::CreateGpu_Vertex(Array<LevelMeshVertex_LM>& vertices)
{
	bufferDesc_t vertex_buffer_desc = {};
	vertex_buffer_desc.type = BUFFERTYPE_VERTEX;
	vertex_buffer_desc.access = BUFFERACCESS_STATIC;
	vertex_buffer_desc.data = &vertices[0];
	vertex_buffer_desc.size = vertices.size() * sizeof(LevelMeshVertex_LM);
	m_vertex_buffer = g_engine->GetRenderDevice()->createBuffer(vertex_buffer_desc);
}

void StaticLevelMesh::CreateGpu_Indices(Array<uint16_t>& indices)
{
	bufferDesc_t index_buffer_desc = {};
	index_buffer_desc.type = BUFFERTYPE_INDEX;
	index_buffer_desc.access = BUFFERACCESS_STATIC;
	index_buffer_desc.data = &indices[0];
	index_buffer_desc.size = indices.size() * sizeof(uint16_t);
	m_index_buffer = g_engine->GetRenderDevice()->createBuffer(index_buffer_desc);
}

void StaticLevelMesh::CreateGpu_Material(const char* material_name, const char* texture_name, const char* lm_name)
{
	const ShaderData shaderData = g_pShaderEngine->LoadShader("lightmapped_generic");
	ASSERT(shaderData.pipelineIndex != INVALID_PIPELINE_INDEX);

	m_pipeline = shaderData.pipelineIndex;

	m_diffuse_texture = g_material_system.LoadTexture(texture_name, true);
	m_lightmap_texture = g_material_system.LoadTexture(lm_name, true);
}

void StaticLevelMesh::DestroyGpu()
{
	//SAFE_DELETE(m_lightmap_texture);
	//SAFE_DELETE(m_diffuse_texture);

	if (m_index_buffer != INVALID_BUFFER_INDEX) {
		g_engine->GetRenderDevice()->deleteBuffer(m_index_buffer);
		m_index_buffer = INVALID_BUFFER_INDEX;
	}

	if (m_vertex_buffer != INVALID_BUFFER_INDEX) {
		g_engine->GetRenderDevice()->deleteBuffer(m_vertex_buffer);
		m_vertex_buffer = INVALID_BUFFER_INDEX;
	}
}

void StaticLevelMesh::Draw(const glm::mat4& model_matrix, const renderContext_t& render_context)
{
	IRenderDevice* render_device = g_engine->GetRenderDevice();

	// set device stuff
	render_device->setPipeline(m_pipeline);

	// fill constants
	render_device->setVSConstant(CONSTANT_MODEL_MATRIX, &model_matrix[0], MATRIX4_SIZE);
	render_device->setVSConstant(CONSTANT_VIEW_MATRIX, &render_context.view_matrix[0], MATRIX4_SIZE);
	render_device->setVSConstant(CONSTANT_PROJ_MATRIX, &render_context.projection_matrix[0], MATRIX4_SIZE);

	glm::mat4 mvp = render_context.GetMVP(model_matrix);
	render_device->setVSConstant(CONSTANT_MVP_MATRIX, &mvp[0], MATRIX4_SIZE);

	render_device->beginBinding();
	g_material_system.SetTexture(0, m_diffuse_texture);
	g_material_system.SetTexture(1, m_lightmap_texture);
	render_device->setVertexBuffer(m_vertex_buffer);
	render_device->setIndexBuffer(m_index_buffer);
	render_device->endBinding();

	render_device->draw(0, m_indices_count, 1);
}

//////////////////
// Dynamic mesh //
//////////////////

DynamicMesh* DynamicMesh::CreateFromStream(IReader* reader)
{
	ASSERT(reader);

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

	char buffer2[_MAX_PATH];
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

	Array<LevelMeshVertex_LM> tempovertices;
	tempovertices.resize(vertices_count);

	reader->read(tempovertices.data(), sizeof(LevelMeshVertex_LM) * vertices_count);

	int counter = 0;
	for (Array<LevelMeshVertex_LM>::iterator it = tempovertices.begin(); it != tempovertices.end(); ++it)
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
	DynamicMesh* mesh = mem_new<DynamicMesh>(tempovertices,
		indices,
		buffer1);

	mem_free(lightmap_texture_path);
	mem_free(diffuse_texture_path);

	return mesh;
}

DynamicMesh::DynamicMesh(Array<LevelMeshVertex_LM>& vertices,
	Array<uint16_t>& indices,
	const char* texture_name)
{
	m_verticesCount = uint32_t(vertices.size());
	m_indices_count = uint32_t(indices.size());

	bufferDesc_t bufferDesc = {};
	bufferDesc.type = BUFFERTYPE_VERTEX;
	bufferDesc.access = BUFFERACCESS_STATIC;
	bufferDesc.data = vertices.data();
	bufferDesc.size = m_verticesCount * sizeof(LevelMeshVertex_LM);

	m_vertexBuffer = g_engine->GetRenderDevice()->createBuffer(bufferDesc);
	ASSERT(m_vertexBuffer != INVALID_BUFFER_INDEX);

	bufferDesc_t index_buffer_desc = {};
	index_buffer_desc.type = BUFFERTYPE_INDEX;
	index_buffer_desc.access = BUFFERACCESS_STATIC;
	index_buffer_desc.data = &indices[0];
	index_buffer_desc.size = indices.size() * sizeof(uint16_t);
	m_index_buffer = g_engine->GetRenderDevice()->createBuffer(index_buffer_desc);
	ASSERT(m_index_buffer != INVALID_BUFFER_INDEX);

	const ShaderData shaderData = g_pShaderEngine->LoadShader("model_test");
	ASSERT(shaderData.pipelineIndex != INVALID_PIPELINE_INDEX);

	m_pipelineIndex = shaderData.pipelineIndex;

	m_tex = g_material_system.LoadTexture(texture_name, true);
}

DynamicMesh::~DynamicMesh()
{
	if (m_vertexBuffer != INVALID_BUFFER_INDEX) {
		g_engine->GetRenderDevice()->deleteBuffer(m_vertexBuffer);
		m_vertexBuffer = INVALID_BUFFER_INDEX;
	}
}

void DynamicMesh::Draw(const glm::mat4& model_matrix, const renderContext_t& render_context)
{
	IRenderDevice* pRenderDevice = g_engine->GetRenderDevice();

	// set device stuff
	pRenderDevice->setPipeline(m_pipelineIndex);

	// fill constants
	pRenderDevice->setVSConstant(CONSTANT_MODEL_MATRIX, &model_matrix[0], MATRIX4_SIZE);
	pRenderDevice->setVSConstant(CONSTANT_VIEW_MATRIX, &render_context.view_matrix[0], MATRIX4_SIZE);
	pRenderDevice->setVSConstant(CONSTANT_PROJ_MATRIX, &render_context.projection_matrix[0], MATRIX4_SIZE);

	glm::mat4 mvp = render_context.GetMVP(model_matrix);
	pRenderDevice->setVSConstant(CONSTANT_MVP_MATRIX, &mvp[0], MATRIX4_SIZE);

	pRenderDevice->beginBinding();
	g_material_system.SetTexture(0, m_tex);
	g_material_system.SetTexture(1, m_tex);
	pRenderDevice->setVertexBuffer(m_vertexBuffer);
	pRenderDevice->setIndexBuffer(m_index_buffer);
	pRenderDevice->endBinding();

	pRenderDevice->draw(0, m_indices_count, 1);
}
