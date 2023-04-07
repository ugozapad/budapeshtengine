#ifndef RENDER_H
#define RENDER_H

struct SDL_Window;

//! Global and unified shader constants
enum uniformConstants_t
{
	CONSTANT_MODEL_MATRIX,
	CONSTANT_VIEW_MATRIX,
	CONSTANT_PROJ_MATRIX,
	CONSTANT_MVP_MATRIX,

	// camera specific
	CONSTANT_CAMERA_EYE,
	CONSTANT_CAMERA_DIR,

	// light specific
	CONSTANT_LIGHT_POS,
	CONSTANT_LIGHT_COLOR,

	// Skinned specific
	CONSTANT_BONE_MATRICES,

	CONSTANT_MAX
};

enum bufferType_t {
	BUFFERTYPE_VERTEX,
	BUFFERTYPE_INDEX,
	BUFFERTYPE_UNIFORM,
	
	BUFFERTYPE_MAX
};

enum bufferAccess_t {
	BUFFERACCESS_STATIC,
	BUFFERACCESS_DYNAMIC,

	BUFFERACCESS_MAX
};

struct bufferDesc_t {
	bufferType_t type;
	bufferAccess_t access;
	void* data;
	size_t size;
};

// Render types
typedef uint32_t bufferIndex_t;

#define INVALID_BUFFER_INDEX -1

//! Interface to renderer
class IRender {
public:
	virtual ~IRender() {}

	virtual void init(SDL_Window* render_window) = 0;
	virtual void shutdown() = 0;

	virtual void renderFrame() = 0;

	// Buffer API

	virtual bufferIndex_t createBuffer(const bufferDesc_t& bufferDesc) = 0;
	virtual void deleteBuffer(bufferIndex_t buffer_index) = 0;
	virtual void updateBuffer(bufferIndex_t buffer_index, void* data, size_t size) = 0;
};

IRender* createRender();

#endif