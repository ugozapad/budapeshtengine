#ifndef TEXTURE_H
#define TEXTURE_H

#include "render/render.h"

class IAllocator;
class IReader;
class IWriter;
class IRender;

class Texture {
public:
	Texture(IAllocator& allocator, IRender& render);
	~Texture();

	void destroy();

	void load(IReader* reader);
	void saveTGA(IWriter* writer);
	void saveJPEG(IWriter* writer);

	textureIndex_t getTextureIndex() { return m_textureIndex; }

private:
	IAllocator& m_allocator;
	IRender& m_render;
	textureIndex_t m_textureIndex;
};

#endif // !TEXTURE_H
