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

void GamePersistent::OnGameStart()
{
	Msg("Welcome to our Budapesht Engine!");

	ASSERT(g_engine->GetLevel());

	Msg("Create player");

	Entity* p_Player = g_object_factory->CreateObject<Player>();
	g_engine->GetLevel()->AddEntity(p_Player);

	// create mesh
	//DynamicMeshEntity* meshEntity = (DynamicMeshEntity*)g_object_factory->CreateObjectByName("dynamic_mesh");
	//meshEntity->SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
	//meshEntity->SetScale(glm::vec3(20.0f));
	//meshEntity->LoadModel("data/levels/test_baking/test_baking.lmf");
	//g_engine->GetLevel()->AddEntity(meshEntity);

	// activate camera
	static_cast<Player*>(p_Player)->activateCamera();
}
