#ifndef LEVEL_H
#define LEVEL_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"

class IReader;

class ENGINE_API Level
{
public:
	Level();
	~Level();

	void load(const char* levelname);
	void loadLMF(IReader* reader);

	Entity* createEntity();
	void addEntity(Entity* entity);

	void render();

	Array<Entity*>& getEntities() { return m_entities; }

private:
	Array<Entity*> m_entities;
};

#endif // !LEVEL_H
