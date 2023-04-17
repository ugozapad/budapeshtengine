#include "engine/level.h"
#include "engine/objectfactory.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

Level::Level(IAllocator& allocator) :
	m_allocator(&allocator),
	m_entities(allocator)
{
}

Level::~Level()
{
}

void Level::load(const char* filepath)
{

}

Entity* Level::createEntity()
{
	Entity* entity = MEM_NEW(*m_allocator, Entity);
	m_entities.push_back(entity);
	return entity;
}

void Level::addEntity(Entity* entity)
{
	if (!entity) {
		printf("Level::addEntity: failed to add null ptr entity\n");
		DebugBreak();
	}

	int entity_count = m_entities.size();
	for (int i = 0; i < entity_count; i++) {
		if (m_entities[i] == entity) {
			printf("Level::addEntity: entity (classname=%s ptr=0x%p) already exists\n", entity->getClassName(), entity);
			DebugBreak();
		}
	}

	m_entities.push_back(entity);
}
