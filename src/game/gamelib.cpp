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
	g_object_factory->RegisterObject<GamePersistent>(CLSID_GAMEPERSISTENT);
	g_object_factory->RegisterObject<WorldSpawn>("world_spawn");
	g_object_factory->RegisterObject<Player>("player");
}

void gameLibInit()
{
	Msg("Registering game objects");
	gameLibRegisterObjects();
}

void gameLibShutdown()
{
}
