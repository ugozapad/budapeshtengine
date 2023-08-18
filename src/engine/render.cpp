#include "pch.h"
#include "render.h"
#include "material_system.h"
#include "debugrender.h"
#include "engine.h"
#include "level.h"
#include "level_mesh.h"
#include "camera.h"

Render g_render;

void Render::Init()
{
	g_material_system.Init();

	// create debug renderer
	g_debugRender = mem_new<DebugRender>();
	g_debugRender->initialize();
}

void Render::Shutdown()
{
	if (g_debugRender) {
		g_debugRender->shutdown();
		SAFE_DELETE(g_debugRender);
	}

	g_material_system.Shutdown();
}

void Render::RenderScene()
{
	// calculate aspect ratio
	viewport_t viewport = g_engine->GetViewport();
	float aspectRatio = (float)viewport.width / (float)viewport.height;

	// calculate projection matrix
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(75.0f), aspectRatio, 0.1f, 1000.0f);

	Camera* camera = g_CameraManager.GetActiveCamera();

	renderContext_t render_context = {};
	render_context.view_matrix = camera ? camera->GetViewMatrix() : glm::mat4(1.0f);
	render_context.projection_matrix = proj;

	// Render static scene
	Array<Entity*>& entities = g_engine->GetLevel()->GetEntities();
	for (auto it : entities)
	{
		if (LevelMesh* level_mesh = dynamicCast<LevelMesh>(it))
		{
			level_mesh->Render(render_context);
		}
	}

	// Render dynamic meshes
	for (auto it : entities)
	{
		if (DynamicMeshEntity* dynamic_mesh = dynamicCast<DynamicMeshEntity>(it))
		{
			dynamic_mesh->Render(render_context);
		}
	}
}
