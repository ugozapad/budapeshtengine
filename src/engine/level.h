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

	void load(const char* levelname);
	void loadLMF(IReader* reader);

	Entity* createEntity();
	void addEntity(Entity* entity); // TODO : Dima to Kirill : Why to give ability to add some random objects to Level?
	void destroyEntity(Entity* entity);

	void update(float fDeltaTime);
	void render();

	Array<Entity*>& getEntities() { return m_entities; }
	PhysicsWorld* getPhysicsWorld();

private:
	bool			m_bBusy;
	bool			m_bNeedToDestroyEnt;
	Array<Entity*>	m_entities;
	PhysicsWorld*	m_physicsWorld;
};

#endif // !LEVEL_H
