#include "pch.h"
#include "render.h"
#include "material_system.h"
#include "debugrender.h"

Render g_render;

void Render::init()
{
	g_material_system.Init();

	// create debug renderer
	g_debugRender = new DebugRender();
	g_debugRender->initialize();
}

void Render::shutdown()
{
	if (g_debugRender) {
		g_debugRender->shutdown();
		SAFE_DELETE(g_debugRender);
	}

	g_material_system.Shutdown();
}
