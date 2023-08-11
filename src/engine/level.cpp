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
#include "engine/debugrender.h"

Level::Level()
	: m_bBusy(false)
{
}

Level::~Level()
{
}

void Level::load(const char* levelname)
{
	Msg("loading level %s", levelname);

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
		//Msg("entity %i %s", i, m_entities[i]->getClassName());

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

void Level::update(float fDeltaTime)
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
	
	for (auto it : m_entities)
	{
		it->update(fDeltaTime);
	}

	m_bBusy = false;
}

void Level::render()
{
	m_bBusy = true;

	for (auto it : m_entities)
	{
		if (LevelMesh* level_mesh = dynamicCast<LevelMesh>(it)) {
			level_mesh->render();
		}
		else if (DynamicMeshEntity* dynamic_entity = dynamicCast<DynamicMeshEntity>(it)) {
			dynamic_entity->render();
		}
	
	}

	m_bBusy = false;
}
