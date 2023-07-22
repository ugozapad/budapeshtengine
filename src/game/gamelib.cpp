#include "pch.h"
#include "gamelib.h"
#include "engine/engine_api.h"
#include "engine/objectfactory.h"
#include "engine/engine.h"
#include "engine/level.h"

#include "worldspawn.h"
#include "player.h"

void gameLibRegisterObjects()
{
	g_object_factory->registerObject<WorldSpawn>();
	g_object_factory->registerObject<Player>();
}

void gameLibInit()
{
	Msg("Registering game objects");

	gameLibRegisterObjects();

	Msg("Create player");

	Entity* pPlayer = g_object_factory->createObject<Player>();
	g_engine->getLevel()->addEntity(pPlayer);
}

void gameLibShutdown()
{
}

IMPLEMENT_ALLOCATOR;