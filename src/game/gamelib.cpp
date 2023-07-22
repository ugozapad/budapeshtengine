#include "pch.h"
#include "gamelib.h"
#include "engine/engine_api.h"
#include "engine/objectfactory.h"
#include "engine/engine.h"
#include "engine/level.h"

#include "worldspawn.h"
#include "player.h"
#include "gamepersistent.h"

void gameLibRegisterObjects()
{
	g_object_factory->registerObject<GamePersistent>("game_persistent");
	g_object_factory->registerObject<WorldSpawn>("world_spawn");
	g_object_factory->registerObject<Player>("player");
}

void gameLibInit()
{
	Msg("Registering game objects");
	gameLibRegisterObjects();
}

void gameLibShutdown()
{
}

IMPLEMENT_ALLOCATOR;