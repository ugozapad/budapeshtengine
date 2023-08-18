#ifndef TEXTURE_H
#define TEXTURE_H

#include "render/irenderdevice.h"

class IAllocator;
class IReader;
class IWriter;
class IRenderDevice;

class Texture {
public:
	Texture();
	~Texture();

	void Destroy();

	void Load(IReader* reader, bool repeat);
	void SaveTGA(IWriter* writer);
	void SaveJPEG(IWriter* writer);

	textureIndex_t GetTextureIndex() { return m_textureIndex; }

private:
	textureIndex_t m_textureIndex;
};

#endif // !TEXTURE_H
