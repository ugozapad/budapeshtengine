#ifndef LEVEL_H
#define LEVEL_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"

class Level
{
public:
	Level(IAllocator& allocator);
	~Level();

	IAllocator& getAllocator() { return *m_allocator; }

	void load(const char* filepath);

	Entity* createEntity();
	void addEntity(Entity* entity);

private:
	IAllocator* m_allocator;
	Array<Entity*> m_entities;
};

#endif // !LEVEL_H
