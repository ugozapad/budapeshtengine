#include "pch.h"
#include "game/gamepersistent.h"
#include "game/player.h"

#include "engine/level.h"
#include "engine/engine.h"
#include "engine/objectfactory.h"

GamePersistent::GamePersistent()
{
}

GamePersistent::~GamePersistent()
{
}

void GamePersistent::onGameStart()
{
	ASSERT(g_engine->getLevel());

	Msg("Create player");

	Entity* pPlayer = g_object_factory->createObject<Player>();
	g_engine->getLevel()->addEntity(pPlayer);
}
