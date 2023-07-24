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

Level::Level()
	: m_bBusy(false)
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
	if (m_bBusy)
	{
#ifdef _DEBUG
		FATAL("Level::createEntity: level is currently busy");
#endif
		return (NULL);
	}

	Entity* entity = new Entity();
	m_entities.push_back(entity);
	return entity;
}

void Level::addEntity(Entity* entity)
{
	if (m_bBusy)
	{
		FATAL("Level::addEntity: level is currently busy");
	}
	if (!entity)
	{
		FATAL("Level::addEntity: failed to add null ptr entity");
	}

	size_t entity_count = m_entities.size();
	for (size_t i = 0; i < entity_count; i++)
	{
		if (m_entities[i] == entity) {
			FATAL("Level::addEntity: entity (classname=%s ptr=0x%p) already exists", entity->getClassName(), entity);
		}
	}

	m_entities.push_back(entity);
}

void Level::destroyEntity(Entity* entity)
{
	if (!entity) return;

	Array<Entity*>::iterator it = std::find(
		m_entities.begin(),
		m_entities.end(),
		entity
	);
	if (it != m_entities.end())
	{
		if (m_bBusy)
		{
			(*it)->setCanBeDestroyed();
			m_bNeedToDestroyEnt = true;
		}
		else
		{
			delete *it;
			m_entities.erase(it);
		}
	}
}

void Level::render()
{
	if (m_bNeedToDestroyEnt)
	{
		struct _destroy_objects_pred
		{
			inline bool operator()(Entity* pE) const
			{
				if (pE->canBeDestroyed())
				{
					delete pE;
					return (true);
				}
				return (false);
			}
		};
		m_entities.erase(
			std::remove_if(
				m_entities.begin(),
				m_entities.end(),
				_destroy_objects_pred()
			),
			m_entities.end()
		);
		m_bNeedToDestroyEnt = false;
	}

	m_bBusy = true;
	for (Array<Entity*>::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
	{
		Entity* entity = (*it);
		if (LevelMesh* level_mesh = dynamicCast<LevelMesh>(entity))
		{
			level_mesh->render();
		}
	}
	m_bBusy = false;
}
