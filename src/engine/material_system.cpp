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
#include "engine/varmanager.h"

enum textureFileFormats_t {
	TEXEXT_PNG,
	TEXEXT_JPG,
	TEXEXT_JPEG,
	TEXEXT_TGA,
	TEXEXT_TIFF,
	TEXEXT_MAX,
};

static const char* s_texture_file_formats[TEXEXT_MAX] = { ".png", ".jpg", ".jpeg", ".tga", ".tiff" };

static Var g_anisotropicValue("r_tex_aniso", "0", "Global anisotropic filter value", VARFLAG_NONE);

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

	// Register vars
	g_VarManager.RegisterVar(&g_anisotropicValue);

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
	char buffer[260];
	bool texfound = false;
	for (int i = 0; i < TEXEXT_MAX; i++) {
		// Kirill: TODO search with file extension for absolute path textures
		if (absolutePath) {
			snprintf(buffer, sizeof(buffer), "%s", filename);
		} else {
			snprintf(buffer, sizeof(buffer), "data/textures/system/notex%s", s_texture_file_formats[i]);
		}
		
		if (g_file_system->fileExist(buffer)) {
			texfound = true;
			break;
		}
	}

	if (!texfound) {
		Msg("not found %s", filename);
		return m_notex;
	}

	// Kirill: TODO shit rewrite
	bool repeat = !strstr(filename, "_lm");

	Texture* p_texture = new Texture(*g_engine->getRenderDevice());

	// open reader
	IReader* reader = g_file_system->openRead(buffer);
	ASSERT(reader);

	p_texture->load(reader, repeat);

	// close reader
	g_file_system->deleteReader(reader);

	Msg("loaded %s", filename);

	return p_texture;
}

Texture* MaterialSystem::GetNoTexture()
{
	return m_notex;
}
