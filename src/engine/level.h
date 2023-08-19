#ifndef LEVEL_H
#define LEVEL_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"
#include "engine/physics_world.h"

class IReader;

class ENGINE_API Level
{
public:
	Level();
	~Level();

	void Load(const char* levelname);
	void LoadLMF(IReader* reader);

	Entity* CreateEntity();
	void AddEntity(Entity* entity);
	void DestroyEntity(Entity* entity);

	void Update(float fDeltaTime);
	void Render();

	Array<Entity*>& GetEntities() { return m_entities; }
	PhysicsWorld*	GetPhysicsWorld();
	const char*		GetLevelName();

private:
	bool			m_bBusy;
	bool			m_bNeedToDestroyEnt;
	Array<Entity*>	m_entities;
	PhysicsWorld*	m_physicsWorld;
	const char*		m_level_name;
};

#endif // !LEVEL_H
