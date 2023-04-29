#ifndef LEVEL_H
#define LEVEL_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"

#include "render/render.h"

class IReader;
class Texture;

struct Vector2 {
	float x, y;
};

struct Vector3 {
	float x, y, z;
};

#define LEVEL_MESH_VERSION 1

struct LevelMeshHeader {
	uint16_t version;
	uint16_t mesh_count;
};

struct LevelMeshVertex_LM {
	Vector3 position;
	Vector2 texcoord0;
	Vector2 texcoord1;
};

//////////////////////////////////////////////
// Level mesh file (.lmf)
//	- LevelMeshHeader
//		- uint16_t version
//		- uint16_t mesh count
//	- each mesh
//		- LevelMeshMaterial
//			- string diffuse texture path
//			- string lightmap texture path
//		- LevelMesh
//			- string mesh name
//			- uint32_t vertices count
//			- array of LevelMeshVertex_LM
//			- uint32_t indices count
//			- array of uint16_t
//
///////////////////////////////////////////////

class LevelMesh : public Entity {
public:
	OBJECT_IMPLEMENT(LevelMesh, Entity);

	LevelMesh();
	~LevelMesh();

	void load(IReader* reader);

	void render();

private:
	void createGpu_Vertex(Array<LevelMeshVertex_LM>& vertices);
	void createGpu_Indices(Array<uint16_t>& indices);

private:
	char* m_mesh_name;

	Texture* m_diffuse_texture;
	Texture* m_lightmap_texture;

	bufferIndex_t m_vertex_buffer;
	bufferIndex_t m_index_buffer;
	uint32_t m_vertices_count;
	uint32_t m_indices_count;
};

class Level
{
public:
	Level(IAllocator& allocator);
	~Level();

	IAllocator& getAllocator() { return *m_allocator; }

	void load(const char* levelname);
	void loadLMF(IReader* reader);

	Entity* createEntity();
	void addEntity(Entity* entity);

	void render();

private:
	IAllocator* m_allocator;
	Array<Entity*> m_entities;
};

#endif // !LEVEL_H
