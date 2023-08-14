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

	Entity* p_Player = g_object_factory->createObject<Player>();
	g_engine->getLevel()->addEntity(p_Player);

	// create mesh
	DynamicMeshEntity* meshEntity = (DynamicMeshEntity*)g_object_factory->createByName("dynamic_mesh");
	meshEntity->loadModel("data/levels/test_baking/test_baking.lmf");
	g_engine->getLevel()->addEntity(meshEntity);

	// activate camera
	static_cast<Player*>(p_Player)->activateCamera();
}
