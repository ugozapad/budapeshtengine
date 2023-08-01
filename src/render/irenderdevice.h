#ifndef RENDER_H
#define RENDER_H

struct SDL_Window;

// Render types
typedef uint32_t bufferIndex_t;
typedef uint32_t shaderIndex_t;
typedef uint32_t pipelineIndex_t;
typedef uint32_t textureIndex_t;

const bufferIndex_t		INVALID_BUFFER_INDEX = -1;
const shaderIndex_t		INVALID_SHADER_INDEX = -1;
const pipelineIndex_t	INVALID_PIPELINE_INDEX = -1;
const textureIndex_t	INVALID_TEXTURE_INDEX = -1;

const int SHADERUNIFORM_MAX_COUNT = 16;

//! Shader uniform size of (in bytes)
const int VECTOR3_SIZE = 12;
const int VECTOR4_SIZE = 16;

const int MATRIX4_SIZE = 64;

const int BONEMATRIXARRAY_MAX_COUNT = 256;
const int BONEMATRIXARRAY_SIZE = MATRIX4_SIZE * BONEMATRIXARRAY_MAX_COUNT;

const int INPUT_LAYOUT_MAX = 8;

//! Global and unified shader constants
enum uniformConstants_t
{
	CONSTANT_MODEL_MATRIX,
	CONSTANT_VIEW_MATRIX,
	CONSTANT_PROJ_MATRIX,
	CONSTANT_MVP_MATRIX,

	// camera specific (vector4)
	CONSTANT_CAMERA_EYE, 
	CONSTANT_CAMERA_DIR,

	// light specific
	CONSTANT_LIGHT_POS,
	CONSTANT_LIGHT_COLOR,

	// Skinned specific
	CONSTANT_BONE_MATRICES
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

enum vertexAttribute_t {
	VERTEXATTR_VEC2,
	VERTEXATTR_VEC3,
	VERTEXATTR_VEC4,

	VERTEXATTR_MAX
};

enum shaderSemantic_t {
	SHADERSEMANTIC_POSITION,
	SHADERSEMANTIC_COLOR,
	SHADERSEMANTIC_TEXCOORD,
	SHADERSEMANTIC_TEXCOORD0,
	SHADERSEMANTIC_TEXCOORD1,
	SHADERSEMANTIC_NORMAL,
	SHADERSEMANTIC_TANGENT,
	SHADERSEMANTIC_BITANGENT,

	SHADERSEMANTIC_MAX
};

enum shaderUniformType_t {
	SHADERUNIFORM_FLOAT,
	SHADERUNIFORM_VEC2,
	SHADERUNIFORM_VEC3,
	SHADERUNIFORM_VEC4,
	SHADERUNIFORM_MAT4,

	SHADERUNIFORM_MAX
};

enum passClearFlags_t {
	PASSCLEAR_COLOR = 1 << 1,
	PASSCLEAR_DEPTH = 1 << 2,
	PASSCLEAR_STENCIL = 1 << 3
};

enum textureType_t {
	TEXTURETYPE_1D,
	TEXTURETYPE_2D,
	TEXTURETYPE_3D,

	TEXTURETYPE_MAX
};

enum textureFormat_t {
	TEXTUREFORMAT_RGB8,
	TEXTUREFORMAT_RGBA8,

	//TEXTUREFORMAT_RGB16F,
	//TEXTUREFORMAT_RGBA16F,
	//TEXTUREFORMAT_RGB32F,
	//TEXTUREFORMAT_RGBA32F,

	TEXTUREFORMAT_MAX
};

struct bufferDesc_t {
	bufferType_t type;
	bufferAccess_t access;
	void* data;
	size_t size;
};

struct inputLayoutDesc_t {
	vertexAttribute_t attribute;
	shaderSemantic_t semantic;
};

struct shaderUniformDesc_t {
	shaderUniformType_t type;
	const char* name;
	size_t size;
};

struct shaderSamplerDesc_t {

};

struct shaderDesc_t {
	const char* vertex_shader_data;
	const char* fragment_shader_data;
	size_t vertex_shader_size;
	size_t fragment_shader_size;

	shaderUniformDesc_t uniform_desc[SHADERUNIFORM_MAX];
	size_t uniform_count;
};

struct pipelineDesc_t {
	inputLayoutDesc_t layouts[INPUT_LAYOUT_MAX];
	size_t layout_count;
	shaderIndex_t shader;
	bool indexed_draw;
};

struct textureDesc_t {
	textureType_t type;
	textureFormat_t format;
	void* data;
	size_t size;
	int width;
	int height;
	int mipmaps_count;
	bool repeat;
};

struct viewport_t {
	int x, y, width, height;
};

class IOcclusionQuery {
public:
	virtual ~IOcclusionQuery() {}

	virtual void begin() = 0;
	virtual void end() = 0;
	virtual bool is_ready() = 0;
};

//! Interface to renderer
class IRenderDevice {
public:
	virtual ~IRenderDevice() {}

	virtual void init(SDL_Window* render_window) = 0;
	virtual void shutdown() = 0;

	virtual void uiFrame() = 0;

	// Draw API

	virtual void draw(int base_element, int num_elements, int num_instances) = 0;

	// Buffer API

	virtual bufferIndex_t createBuffer(const bufferDesc_t& buffer_desc) = 0;
	virtual void deleteBuffer(bufferIndex_t buffer_index) = 0;
	virtual void updateBuffer(bufferIndex_t buffer_index, void* data, size_t size) = 0;

	// Shader API
	
	virtual shaderIndex_t createShader(const shaderDesc_t& shader_desc) = 0;
	virtual void deleteShader(shaderIndex_t shader) = 0;

	virtual void setVSConstant(int ub_index, const void* data, size_t size) = 0;
	virtual void setPSConstant(int ub_index, const void* data, size_t size) = 0;

	// Pipeline API

	virtual pipelineIndex_t createPipeline(const pipelineDesc_t& pipeline_desc) = 0;
	virtual void deletePipeline(pipelineIndex_t pipeline) = 0;

	// Texture API

	virtual textureIndex_t createTexture(const textureDesc_t& texture_desc) = 0;
	virtual void deleteTexture(textureIndex_t texture) = 0;

	// Binding API

	virtual void beginBinding() = 0;

	virtual void setVertexBuffer(bufferIndex_t buffer_index) = 0;
	virtual void setIndexBuffer(bufferIndex_t buffer_index) = 0;
	virtual void setPipeline(pipelineIndex_t pipeline) = 0;
	virtual void setTexture(int index, textureIndex_t texture) = 0;

	virtual void endBinding() = 0;

	// Pass API
	virtual void beginPass(const viewport_t& viewport, int pass_clear_flags) = 0;
	virtual void endPass() = 0;

	// Core API

	virtual void commit() = 0;

	virtual void present(bool vsync) = 0;

	virtual IOcclusionQuery* createOcclusionQuery() = 0;
};

#ifndef ENGINE_EXPORTS
extern "C"
{
	__declspec(dllexport) IRenderDevice* createRenderDevice();
};
#else
typedef IRenderDevice*(__stdcall *createRenderDevice_t)();
#endif

#endif