#ifndef level_mesh_h__
#define level_mesh_h__
#pragma once

#define LEVEL_MESH_VERSION 1
#define MAX_MESHES_ON_LEVEL 0xFFFF

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

#pragma pack(push)
#pragma pack(1)
struct LevelMeshHeader
{
	uint16_t version;
	uint16_t mesh_count;
};

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
#pragma pack(pop)

#endif // level_mesh_h__
