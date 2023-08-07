// fontsystem.cpp - Implementation of font system.
#include "pch.h"
#include "render/irenderdevice.h"
#include "engine.h"
#include "fontsystem.h"
#include "varmanager.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// font vertex
struct FontVertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec4 color;
};

const int MAX_FONT_VBO_SIZE = sizeof(FontVertex) * 6 * 2048;

// Global variables
ENGINE_API FontSystem g_fontSystem;
Var g_fontDrawDbgAlpha("r_font_dbg_alpha", "0", "", VARFLAG_DEVELOPER_MODE);

//////////////////////////////////////////////////////////////////////////
// Font
//////////////////////////////////////////////////////////////////////////
#define SYMBOL_COORDS_SECT "symbol_coordinates"
#define DIV(a, b) (float)(((float)(a)) / ((float)(b)))
Font::Font(const char* sect)
{
#if 0
	m_dwHeight = Configs().r_u32(sect, "height");
	if (Height() > 40)
		m_dwTextureWidth = 1024;
	else if (Height() > 20)
		m_dwTextureWidth = 512;
	else
		m_dwTextureWidth = 256;
	m_dwTextureHeight = m_dwTextureWidth;

	LPCSTR strTexture = Configs().r_string(sect, "texture");

	std::string fp; g_fileManager->getPath("textures", strTexture, fp);
	m_pFontMaterial = g_materialsManager->createMaterial("system_console_font", fp.c_str());

	DECLARE_STRING(buf, 1024); strcpy(buf, fp.c_str());
	if (char* pExt = strrchr(buf, '.')) *pExt = '\0';
	strcat(buf, ".ini");

	INIFile* pIni = new INIFile(buf);
	DECLARE_STRING(tmp, 8);
	for (uint16_t i = 0; i < 256; ++i)
	{
		snprintf(tmp, sizeof(tmp), "%03u", i);
		LPCSTR val = pIni->r_string(SYMBOL_COORDS_SECT, tmp);

		uint32_t coords[4];
		if (sscanf(val, "%u, %u, %u, %u", &coords[0], &coords[1], &coords[2], &coords[3]) == 4)
		{
			m_TextureCoords[i] = {
				{ DIV(coords[0], m_dwTextureWidth), DIV(coords[1], m_dwTextureHeight) },
				{ DIV(coords[2], m_dwTextureWidth), DIV(coords[3], m_dwTextureHeight) }
			};
		}
		else {
			FATAL("Invalid font coords %s", sect);
		}
	}
	delete pIni;
#endif
}

Font::~Font()
{

}

//////////////////////////////////////////////////////////////////////////
// Font system
//////////////////////////////////////////////////////////////////////////
FontSystem::FontSystem()
{
	m_vertexBuffer = INVALID_BUFFER_INDEX;
}

FontSystem::~FontSystem()
{
}

void FontSystem::initialize()
{
	g_VarManager.RegisterVar(&g_fontDrawDbgAlpha);

	// Create dynamic vertex buffer
	
	bufferDesc_t buffer_desc = {};
	buffer_desc.type = BUFFERTYPE_VERTEX;
	buffer_desc.access = BUFFERACCESS_DYNAMIC;
	buffer_desc.size = MAX_FONT_VBO_SIZE;
	
	m_vertexBuffer = g_engine->getRenderDevice()->createBuffer(buffer_desc);
}

void FontSystem::shutdown()
{
	if (m_vertexBuffer)	{
		g_engine->getRenderDevice()->deleteBuffer(m_vertexBuffer);
		m_vertexBuffer = INVALID_BUFFER_INDEX;
	}

	FONTS_LIST::iterator	I = m_loadedFonts.begin(),
							E = m_loadedFonts.end();
	for (; I != E; ++I)
		delete I->second;
	
	m_loadedFonts.clear();
}

Font* FontSystem::loadFont(const char* name)
{
	std::string short_name = name;
	Font* pFont = NULL;
	FONTS_LIST::iterator it = m_loadedFonts.find(short_name);
	if (it != m_loadedFonts.end())
		pFont = it->second;
	else
		m_loadedFonts[short_name] = pFont = new Font(name);
	return pFont;
}

void FontSystem::drawText(Font* font, const char* text, float x, float y, Fcolor color)
{
	IRenderDevice* pRenderDevice = g_engine->getRenderDevice();

	int buffer_size = strlen(text) * sizeof(FontVertex);

	// grow buffer
	static int last_capacity = 0;
	if (last_capacity < buffer_size) {
	
		pRenderDevice->deleteBuffer(m_vertexBuffer);

		bufferDesc_t buffer_desc = {};
		buffer_desc.type = BUFFERTYPE_VERTEX;
		buffer_desc.access = BUFFERACCESS_DYNAMIC;
		buffer_desc.size = buffer_size;

		m_vertexBuffer = pRenderDevice->createBuffer(buffer_desc);
	}

#if 0
	glm::vec2 const orig_pos(x, y);
	glm::vec2		draw_pos(orig_pos);
	const float fSymbolHeight = font->SymbolHeight();
	const float fTextureScale = 1.0f;
	uint32_t verticesCount = 0;

	FontVertex* vertices = (FontVertex*)mem_alloc(buffer_size, alignof(FontVertex));

	// fill vertex buffer
	for (int i = 0; i < strlen(text); i++) {
		uint8_t const c = *text;
		if (c == '\r') continue;
		if (c == '\n')
		{
			draw_pos.x = orig_pos.x;
			draw_pos.y += fSymbolHeight;
			continue;
		}

		Fbox2 const& fbCoords = font->GetCoord(c);
		const float fW = (fbCoords.max.x - fbCoords.min.x) * (font->TextureWidth() / fTextureScale);
		const float fH = (fbCoords.max.y - fbCoords.min.y) * (font->TextureHeight() / fTextureScale);
		if (c != ' ')
		{
			*vertices++ = { { draw_pos.x + 0.f - 0.5f	, draw_pos.y + fH - 0.5f	, 0.f }, { fbCoords.min.x, -fbCoords.min.y }, vxColor };
			*vertices++ = { { draw_pos.x + 0.f - 0.5f	, draw_pos.y + 0.f - 0.5f	, 0.f }, { fbCoords.min.x, -fbCoords.max.y }, vxColor };
			*vertices++ = { { draw_pos.x + fW - 0.5f	, draw_pos.y + fH - 0.5f	, 0.f }, { fbCoords.max.x, -fbCoords.min.y }, vxColor };
			*vertices++ = { { draw_pos.x + fW - 0.5f	, draw_pos.y + 0.f - 0.5f	, 0.f }, { fbCoords.max.x, -fbCoords.max.y }, vxColor };
			*vertices++ = { { draw_pos.x + fW - 0.5f	, draw_pos.y + fH - 0.5f	, 0.f }, { fbCoords.max.x, -fbCoords.min.y }, vxColor };
			*vertices++ = { { draw_pos.x + 0.f - 0.5f	, draw_pos.y + 0.f - 0.5f	, 0.f }, { fbCoords.min.x, -fbCoords.max.y }, vxColor };
			verticesCount += 2;

			if ((verticesCount * 3) > (MAX_FONT_VBO_SIZE - 6))
			{
				m_vertexBuffer->unmapBuffer();
				drawVertices(m_vertexBuffer, verticesCount * 3, font->GetMaterial());
				vertices = static_cast<FontVertex*>(m_vertexBuffer->mapBuffer(BufferAccess::WRITE_ONLY));
				verticesCount = 0;
			}
		}

		draw_pos.x += fW;
	}
#endif

#if 0
	//////////////////////////////////////////
	// view stuff

	// copy view
	View fontView = g_render->topView();
	fontView.proj = glm::ortho(0.0f, (float)fontView.width, 0.0f, (float)fontView.height);
	fontView.view = glm::identity<glm::mat4>();
	g_render->pushView(fontView);

	// bind vertex buffer
	g_renderDevice->setVerticesBuffer(m_vertexBuffer);

	// map buffer
	FontVertex* vertices = static_cast<FontVertex*>(m_vertexBuffer->mapBuffer(BufferAccess::WRITE_ONLY));

	glm::vec2 const orig_pos(x, y);
	glm::vec2		draw_pos(orig_pos);
	const float fSymbolHeight = font->SymbolHeight();
	const float fTextureScale = 1.0f;
	uint32_t verticesCount = 0;

	glm::vec4 const vxColor = glm::vec4(
		float(GET_R(color)) / 255.f,
		float(GET_G(color)) / 255.f,
		float(GET_B(color)) / 255.f,
		float(GET_A(color)) / 255.f
	);

	for (; *text; ++text)
	{
		uint8_t const c = *text;
		if (c == '\r') continue;
		if (c == '\n')
		{
			draw_pos.x = orig_pos.x;
			draw_pos.y += fSymbolHeight;
			continue;
		}

		Fbox2 const& fbCoords = font->GetCoord(c);
		const float fW = (fbCoords.max.x - fbCoords.min.x) * (font->TextureWidth() / fTextureScale);
		const float fH = (fbCoords.max.y - fbCoords.min.y) * (font->TextureHeight() / fTextureScale);
		if (c != ' ')
		{
			*vertices++ = { { draw_pos.x + 0.f - 0.5f	, draw_pos.y + fH - 0.5f	, 0.f }, { fbCoords.min.x, -fbCoords.min.y }, vxColor };
			*vertices++ = { { draw_pos.x + 0.f - 0.5f	, draw_pos.y + 0.f - 0.5f	, 0.f }, { fbCoords.min.x, -fbCoords.max.y }, vxColor };
			*vertices++ = { { draw_pos.x + fW - 0.5f	, draw_pos.y + fH - 0.5f	, 0.f }, { fbCoords.max.x, -fbCoords.min.y }, vxColor };
			*vertices++ = { { draw_pos.x + fW - 0.5f	, draw_pos.y + 0.f - 0.5f	, 0.f }, { fbCoords.max.x, -fbCoords.max.y }, vxColor };
			*vertices++ = { { draw_pos.x + fW - 0.5f	, draw_pos.y + fH - 0.5f	, 0.f }, { fbCoords.max.x, -fbCoords.min.y }, vxColor };
			*vertices++ = { { draw_pos.x + 0.f - 0.5f	, draw_pos.y + 0.f - 0.5f	, 0.f }, { fbCoords.min.x, -fbCoords.max.y }, vxColor };
			verticesCount += 2;

			if ((verticesCount * 3) > (MAX_FONT_VBO_SIZE - 6))
			{
				m_vertexBuffer->unmapBuffer();
				drawVertices(m_vertexBuffer, verticesCount * 3, font->GetMaterial());
				vertices = static_cast<FontVertex*>(m_vertexBuffer->mapBuffer(BufferAccess::WRITE_ONLY));
				verticesCount = 0;
			}
		}

		draw_pos.x += fW;
	}

	// unmap buffer
	m_vertexBuffer->unmapBuffer();

	drawVertices(m_vertexBuffer, verticesCount * 3, font->GetMaterial());

	// return view settings back
	g_render->popView();
#endif
}
