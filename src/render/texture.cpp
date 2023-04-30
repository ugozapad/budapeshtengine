#include "engine/allocator.h"
#include "engine/filesystem.h"
#include "render/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Texture::Texture(IAllocator& allocator, IRenderDevice& render) :
	m_allocator(allocator)
,	m_render(render)
,	m_textureIndex(INVALID_TEXTURE_INDEX)
{
}

Texture::~Texture() {
	destroy();
}

void Texture::destroy() {
	if (m_textureIndex != INVALID_TEXTURE_INDEX) {
		m_render.deleteTexture(m_textureIndex);
		m_textureIndex = INVALID_TEXTURE_INDEX;
	}
}

void Texture::load(IReader* reader, bool repeat) {
	reader->seek(SeekWay::End, 0);
	size_t length = reader->tell();
	reader->seek(SeekWay::Begin, 0);

	// allocate image buffer
	uint8_t* image_buffer = (uint8_t*)m_allocator.allocate(length, 4);

	// read file
	reader->read(image_buffer, length);

	stbi_set_flip_vertically_on_load(true);

	// stb image
	int width, height, channels;
	uint8_t* image_data = stbi_load_from_memory(image_buffer, int(length), &width, &height, &channels, STBI_rgb_alpha);

	// create gpu texture
	textureDesc_t texture_desc = {};
	texture_desc.type = TEXTURETYPE_2D;
	texture_desc.format = TEXTUREFORMAT_RGBA8;
	texture_desc.data = image_data;
	texture_desc.width = width;
	texture_desc.height = height;
	texture_desc.size = size_t(width) * size_t(height) * 4;
	texture_desc.mipmaps_count = 0;
	texture_desc.repeat = repeat;
	m_textureIndex = m_render.createTexture(texture_desc);

	// validate texture index
	if (m_textureIndex == INVALID_TEXTURE_INDEX) {
		// ERROR: Some parameters of texture_desc is invalid or pool is out
		__debugbreak();
	}

	// free stbi memory
	STBI_FREE(image_data);

	// free file memory
	m_allocator.deallocate(image_buffer);
}

// stb image write callback
static void stbiWriteFunc(void* context, void* data, int size) {
	IWriter* writer = (IWriter*)context;
	writer->write(data, size);
}

void Texture::saveTGA(IWriter* writer) {
	stbi_write_tga_to_func(stbiWriteFunc, writer, 128, 128, 128, nullptr);
}

void Texture::saveJPEG(IWriter* writer) {
	stbi_write_jpg_to_func(stbiWriteFunc, writer, 128, 128, 128, nullptr, 80);
}
