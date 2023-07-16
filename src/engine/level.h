#ifndef LEVEL_H
#define LEVEL_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"

class IReader;

class Level
{
public:
	Level();
	~Level();

	void load(const char* levelname);
	void loadLMF(IReader* reader);

	Entity* createEntity();
	void addEntity(Entity* entity);

	void render();

private:
	Array<Entity*> m_entities;
};

#endif // !LEVEL_H
