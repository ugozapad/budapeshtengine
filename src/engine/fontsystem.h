// fontsystem.h - Font system
#ifndef FONTSYSTEM_H
#define FONTSYSTEM_H

#include <map>

template <typename T>
struct _color {
	union 
	{
		struct
		{
			T r;
			T g;
			T b;
			T a;
		};

		uint32_t rgba;
	};
};

typedef _color<float> Fcolor;

class ENGINE_API Font
{
public:
	Font(const char* texName);
	~Font();
};

class ENGINE_API FontSystem
{
	typedef std::map<std::string, Font*> FONTS_LIST;
public:
	FontSystem();
	~FontSystem();

	void		initialize();
	void		shutdown();

	Font*		loadFont(const char* name);

	void		drawText(Font* font, const char* text, float x, float y, Fcolor color);

private:
	bufferIndex_t	m_vertexBuffer;
	FONTS_LIST		m_loadedFonts;
};

extern ENGINE_API FontSystem g_fontSystem;

#endif // !FONTSYSTEM_H
