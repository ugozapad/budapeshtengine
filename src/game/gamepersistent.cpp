#include "pch.h"
#include "game/gamepersistent.h"
#include "game/player.h"

#include "engine/level.h"
#include "engine/engine.h"
#include "engine/level_mesh.h"
#include "engine/objectfactory.h"

GamePersistent::GamePersistent()
{
}

GamePersistent::~GamePersistent()
{
}

void GamePersistent::onGameStart()
{
	Msg("Welcome to our Budapesht Engine!");

	ASSERT(g_engine->getLevel());

	Msg("Create player");

	Entity* pPlayer = g_object_factory->createObject<Player>(); ASSERT(pPlayer);
	g_engine->getLevel()->addEntity(pPlayer);

	// create mesh
	DynamicMeshEntity* meshEntity = dynamic_cast<DynamicMeshEntity*>(g_object_factory->createByName("dynamic_mesh")); ASSERT(meshEntity);
	meshEntity->loadModel("data/levels/test_baking/test_baking.lmf");
	g_engine->getLevel()->addEntity(meshEntity);
}
