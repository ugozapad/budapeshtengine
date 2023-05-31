#ifndef MESH_H
#define MESH_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "render/irenderdevice.h"

#include <glm/glm.hpp>

class Texture;

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

struct LevelMeshVertex_LM
{
	Vector3 position;
	Vector2 texcoord0;
	Vector2 texcoord1;
};

struct renderContext_t
{
	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

	const glm::mat4 getMVP(const glm::mat4& model_matrix) const
	{
		glm::mat4 mvp = glm::mat4(1.0f);
		mvp = projection_matrix * view_matrix * model_matrix;
		return mvp;
	}
};

class StaticMesh
{
public:
	StaticMesh(IAllocator& allocator, 
		Array<LevelMeshVertex_LM>& vertices, 
		Array<uint16_t>& indices, 
		const char* material_name, 
		const char* texture_name, 
		const char* lm_name);

	~StaticMesh();

	void draw(const glm::mat4& model_matrix, const renderContext_t& render_context);

private:
	void createGpu_Vertex(Array<LevelMeshVertex_LM>& vertices);
	void createGpu_Indices(Array<uint16_t>& indices);
	void createGpu_Material(const char* material_name,
		const char* texture_name,
		const char* lm_name);

	void destroyGpu();

private:
	IAllocator* m_allocator;

	Texture* m_diffuse_texture;
	Texture* m_lightmap_texture;

	pipelineIndex_t m_pipeline;

	bufferIndex_t m_vertex_buffer;
	bufferIndex_t m_index_buffer;

	uint32_t m_vertices_count;
	uint32_t m_indices_count;
};

#endif // !MESH_H
