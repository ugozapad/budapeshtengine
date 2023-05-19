#ifndef TEXTURE_H
#define TEXTURE_H

#include "render/irenderdevice.h"

class IAllocator;
class IReader;
class IWriter;
class IRenderDevice;

class Texture {
public:
	Texture(IAllocator& allocator, IRenderDevice& render);
	~Texture();

	void destroy();

	void load(IReader* reader, bool repeat);
	void saveTGA(IWriter* writer);
	void saveJPEG(IWriter* writer);

	textureIndex_t getTextureIndex() { return m_textureIndex; }

private:
	IAllocator* m_allocator;
	IRenderDevice* m_render;
	textureIndex_t m_textureIndex;
};

#endif // !TEXTURE_H
