#ifndef MESH_H
#define MESH_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "render/irenderdevice.h"

#include <glm/glm.hpp>

class Texture;
class IReader;

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

template <typename T>
struct TVector4
{
	T x, y, z, w;
};

typedef TVector4<float> Vector4;
typedef TVector4<uint32_t> Vector4i;

struct LevelMeshVertex_LM
{
	Vector3 position;
	Vector2 texcoord0;
	Vector2 texcoord1;
};

struct DynamicMeshVertex
{
	Vector3 position;
	Vector3 normal;
	Vector2 texcoord;
};

struct SkeletonMeshVertex
{
	Vector3 position;
	Vector3 normal;
	Vector2 texcoord;
	Vector4i boneIds;
	Vector4 weights;
};

typedef struct {
	Vector3 min;
	Vector3 max;
} boundingBox_t;

class IRenderable {
public:
	virtual ~IRenderable();

	// Kirill: to think, Entity::isActive?
	/*virtual void setEnable(bool value) = 0;
	virtual bool getEnable() = 0;*/

	virtual void getBoundingBox(boundingBox_t& bounding_box) = 0;
	virtual void getWorldBoundingBox(boundingBox_t& bounding_box) = 0;
};

inline IRenderable::~IRenderable()
{
}

struct renderContext_t
{
	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

	const glm::mat4 GetMVP(const glm::mat4& model_matrix) const
	{
		glm::mat4 mvp = glm::mat4(1.0f);
		mvp = projection_matrix * view_matrix * model_matrix;
		return mvp;
	}
};

class StaticLevelMesh
{
public:
	StaticLevelMesh(Array<LevelMeshVertex_LM>& vertices, 
		Array<uint16_t>& indices, 
		const char* material_name, 
		const char* texture_name, 
		const char* lm_name);

	~StaticLevelMesh();

	void Draw(const glm::mat4& model_matrix, const renderContext_t& render_context);

private:
	void CreateGpu_Vertex(Array<LevelMeshVertex_LM>& vertices);
	void CreateGpu_Indices(Array<uint16_t>& indices);
	void CreateGpu_Material(const char* material_name,
		const char* texture_name,
		const char* lm_name);

	void DestroyGpu();

private:
	Texture* m_diffuse_texture;
	Texture* m_lightmap_texture;

	pipelineIndex_t m_pipeline;

	bufferIndex_t m_vertex_buffer;
	bufferIndex_t m_index_buffer;

	uint32_t m_vertices_count;
	uint32_t m_indices_count;
};

class DynamicMesh
{
public:
	static DynamicMesh* CreateFromStream(IReader* reader);

public:
	DynamicMesh(Array<LevelMeshVertex_LM>& vertices,
		Array<uint16_t>& indices,
		const char* texture_name);
	~DynamicMesh();

	void Draw(const glm::mat4& model_matrix, const renderContext_t& render_context);

private:
	Texture* m_tex;
	
	pipelineIndex_t m_pipelineIndex;

	bufferIndex_t m_vertexBuffer;
	bufferIndex_t m_index_buffer;

	uint32_t m_verticesCount;
	uint32_t m_indices_count;

};

#endif // !MESH_H
