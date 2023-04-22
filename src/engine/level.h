#ifndef LEVEL_H
#define LEVEL_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"

#include "render/render.h"

class IReader;

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
	void createGpu(Array<LevelMeshVertex_LM>& vertices);

private:
	char* m_mesh_name;

	bufferIndex_t m_vertex_buffer;
	bufferIndex_t m_index_buffer;
	uint32_t m_vertices_count;
};

class Level
{
public:
	Level(IAllocator& allocator);
	~Level();

	IAllocator& getAllocator() { return *m_allocator; }

	void load(const char* filepath);
	void loadLMF(const char* filepath);

	Entity* createEntity();
	void addEntity(Entity* entity);

private:
	IAllocator* m_allocator;
	Array<Entity*> m_entities;
};

#endif // !LEVEL_H
