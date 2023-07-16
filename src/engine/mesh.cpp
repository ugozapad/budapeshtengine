#include "engine/mesh.h"
#include "engine/engine.h"
#include "engine/shader_engine.h"
#include "engine/texture.h"
#include "engine/material_system.h"

StaticMesh::StaticMesh(Array<LevelMeshVertex_LM>& vertices, 
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

	m_vertices_count = vertices.size();
	m_indices_count = indices.size();

	createGpu_Vertex(vertices);
	createGpu_Indices(indices);
	createGpu_Material(material_name, texture_name, lm_name);
}

StaticMesh::~StaticMesh()
{
	destroyGpu();
}

void StaticMesh::createGpu_Vertex(Array<LevelMeshVertex_LM>& vertices)
{
	bufferDesc_t vertex_buffer_desc = {};
	vertex_buffer_desc.type = BUFFERTYPE_VERTEX;
	vertex_buffer_desc.access = BUFFERACCESS_STATIC;
	vertex_buffer_desc.data = &vertices[0];
	vertex_buffer_desc.size = vertices.size() * sizeof(LevelMeshVertex_LM);
	m_vertex_buffer = g_engine->getRenderDevice()->createBuffer(vertex_buffer_desc);
}

void StaticMesh::createGpu_Indices(Array<uint16_t>& indices)
{
	bufferDesc_t index_buffer_desc = {};
	index_buffer_desc.type = BUFFERTYPE_INDEX;
	index_buffer_desc.access = BUFFERACCESS_STATIC;
	index_buffer_desc.data = &indices[0];
	index_buffer_desc.size = indices.size() * sizeof(uint16_t);
	m_index_buffer = g_engine->getRenderDevice()->createBuffer(index_buffer_desc);
}

void StaticMesh::createGpu_Material(const char* material_name, const char* texture_name, const char* lm_name)
{
	const ShaderData shaderData = g_pShaderEngine->loadShader("lightmapped_generic");
	ASSERT(shaderData.pipelineIndex != INVALID_PIPELINE_INDEX);

	m_pipeline = shaderData.pipelineIndex;

	m_diffuse_texture = g_material_system.LoadTexture(texture_name);
	m_lightmap_texture = g_material_system.LoadTexture(lm_name);
}

void StaticMesh::destroyGpu()
{
	if (m_index_buffer != INVALID_BUFFER_INDEX) {
		g_engine->getRenderDevice()->deleteBuffer(m_index_buffer);
		m_index_buffer = INVALID_BUFFER_INDEX;
	}

	if (m_vertex_buffer != INVALID_BUFFER_INDEX) {
		g_engine->getRenderDevice()->deleteBuffer(m_vertex_buffer);
		m_vertex_buffer = INVALID_BUFFER_INDEX;
	}
}

void StaticMesh::draw(const glm::mat4& model_matrix, const renderContext_t& render_context)
{
	IRenderDevice* render_device = g_engine->getRenderDevice();

	// set device stuff
	render_device->setPipeline(m_pipeline);

	// fill constants
	render_device->setVSConstant(CONSTANT_MODEL_MATRIX, &model_matrix[0], MATRIX4_SIZE);
	render_device->setVSConstant(CONSTANT_VIEW_MATRIX, &render_context.view_matrix[0], MATRIX4_SIZE);
	render_device->setVSConstant(CONSTANT_PROJ_MATRIX, &render_context.projection_matrix[0], MATRIX4_SIZE);

	glm::mat4 mvp = render_context.getMVP(model_matrix);
	render_device->setVSConstant(CONSTANT_MVP_MATRIX, &mvp[0], MATRIX4_SIZE);

	render_device->beginBinding();
	render_device->setTexture(0, m_diffuse_texture->getTextureIndex());
	render_device->setTexture(1, m_lightmap_texture->getTextureIndex());
	render_device->setVertexBuffer(m_vertex_buffer);
	render_device->setIndexBuffer(m_index_buffer);
	render_device->endBinding();

	render_device->draw(0, m_indices_count, 1);
}
