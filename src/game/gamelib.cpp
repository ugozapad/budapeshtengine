#include "pch.h"
#include "gamelib.h"
#include "engine/engine_api.h"
#include "engine/objectfactory.h"

#include "worldspawn.h"
#include "player.h"

void gameLibRegisterObjects()
{
	g_object_factory->registerObject<WorldSpawn>();
	g_object_factory->registerObject<Player>();
}

void gameLibInit()
{
	gameLibRegisterObjects();
}

void gameLibShutdown()
{
}

IMPLEMENT_ALLOCATOR;