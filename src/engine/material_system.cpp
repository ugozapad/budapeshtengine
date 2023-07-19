#include "pch.h"
#include "engine/debug.h"
#include "engine/allocator.h"
#include "engine/filesystem.h"
#include "engine/logger.h"
#include "engine/engine.h"
#include "engine/material_system.h"
#include "engine/texture.h"
#include "render/irenderdevice.h"
#include "engine/shader_engine.h"

enum textureFileFormats_t {
	TEXEXT_PNG,
	TEXEXT_JPG,
	TEXEXT_JPEG,
	TEXEXT_TGA,
	TEXEXT_TIFF,
	TEXEXT_MAX,
};

static const char* s_texture_file_formats[TEXEXT_MAX] = { ".png", ".jpg", ".jpeg", ".tga", ".tiff" };

MaterialSystem g_material_system;

MaterialSystem::MaterialSystem() :
	m_notex(nullptr)
{
}

MaterialSystem::~MaterialSystem()
{
}

void MaterialSystem::Init()
{
	Msg("Material System initialization ...");

	// Initialize shader engine
	g_pShaderEngine = new ShaderEngine("gl33");

	char buffer[260];
	bool texfound = false;
	for (int i = 0; i < TEXEXT_MAX; i++) {
		snprintf(buffer, sizeof(buffer), "data/textures/system/notex%s", s_texture_file_formats[i]);
		if (g_file_system->fileExist(buffer)) {
			texfound = true;
			break;
		}
	}

	if (!texfound) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Very serious problem!",
			"Failed to file 'data/textures/system/notex.tga'.", nullptr);
		exit(-1);
	}

	m_notex = new Texture(*g_engine->getRenderDevice());

	// open reader
	IReader* reader = g_file_system->openRead(buffer);
	ASSERT(reader);

	m_notex->load(reader, false);

	// close reader
	g_file_system->deleteReader(reader);
}

void MaterialSystem::Shutdown()
{
	if (m_notex)
	{
		delete m_notex;
		m_notex = nullptr;
	}

	SAFE_DELETE(g_pShaderEngine);
}

Texture* MaterialSystem::LoadTexture(const char* filename, bool absolutePath /*= false*/)
{
	return nullptr;
}

Texture* MaterialSystem::GetNoTexture()
{
	return m_notex;
}
