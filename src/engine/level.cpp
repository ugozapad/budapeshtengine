#include "pch.h"
#include "engine/level.h"
#include "engine/objectfactory.h"
#include "engine/filesystem.h"
#include "engine/engine.h"
#include "engine/camera.h"
#include "engine/debug.h"
#include "engine/shader_engine.h"
#include "engine/level_mesh.h"
#include "engine/texture.h"
#include "engine/igamepersistent.h"

#include <glm/glm.hpp>

#include <stdio.h>

Level::Level()
{
}

Level::~Level()
{
}

void Level::load(const char* levelname)
{
	IReader* reader;
	char levelpath[512];
	snprintf(levelpath, 512, "data/levels/%s/", levelname);
	size_t const pathend = strlen(levelpath);
	
	strcpy(levelpath + pathend, "level.lmf");
	reader = g_file_system->openRead(levelpath);
	loadLMF(reader);
	g_file_system->deleteReader(reader);
	
	// strcpy(levelpath + pathend, "level.somefile");
	// reader = g_file_system->openRead(levelpath);
	// loadSomeFile(reader);
	// etc.

	g_pGamePersistent->onGameStart();
}

void Level::loadLMF(IReader* reader)
{
	// read header
	LevelMeshHeader header;
	reader->read(&header, sizeof(header));

	for (uint16_t i = 0; i < header.mesh_count; i++)
	{
		LevelMesh* level_mesh = new LevelMesh();
		level_mesh->load(reader);

		Entity* level_mesh_entity = level_mesh;
		m_entities.push_back(level_mesh_entity);
	}
}

Entity* Level::createEntity()
{
	Entity* entity = new Entity();
	m_entities.push_back(entity);
	return entity;
}

void Level::addEntity(Entity* entity)
{
	if (!entity)
	{
		FATAL("Level::addEntity: failed to add null ptr entity");
	}

	size_t entity_count = m_entities.size();
	for (size_t i = 0; i < entity_count; i++)
	{
		//Msg("entity %i %s", i, m_entities[i]->getClassName());

		if (m_entities[i] == entity) {
			FATAL("Level::addEntity: entity (classname=%s ptr=0x%p) already exists", entity->getClassName(), entity);
		}
	}

	m_entities.push_back(entity);
}

void Level::render()
{
	for (Array<Entity*>::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
	{
		Entity* entity = (*it);

		entity->update(0.003f);

		if (LevelMesh* level_mesh = dynamicCast<LevelMesh>(entity))
		{
			level_mesh->render();
		}
	}
}
