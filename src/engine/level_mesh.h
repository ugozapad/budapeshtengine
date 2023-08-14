#ifndef LEVEL_MESH_H
#define LEVEL_MESH_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"
#include "engine/mesh.h"

#include "render/irenderdevice.h"

#define LEVEL_MESH_VERSION 1

class IReader;
class Texture;

struct LevelMeshHeader
{
	uint16_t version;
	uint16_t mesh_count;
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

class ENGINE_API LevelMesh : public Entity {
public:
	OBJECT_IMPLEMENT(LevelMesh, Entity);

	LevelMesh();
	~LevelMesh();

	void load(IReader* reader);

	void render(const renderContext_t& render_context);

private:
	char* m_mesh_name;
	StaticLevelMesh* m_mesh;
};

// Kirill: TEMP WAY PLEASE REMOVE ASAP
class ENGINE_API DynamicMeshEntity : public Entity {
public:
	OBJECT_IMPLEMENT(DynamicMeshEntity, Entity);

	DynamicMeshEntity();
	~DynamicMeshEntity();

	void loadModel(const char* filename);

	void render(const renderContext_t& render_context);

private:
	Array<DynamicMesh*> m_meshes;
};

#endif // !LEVEL_MESH_H
