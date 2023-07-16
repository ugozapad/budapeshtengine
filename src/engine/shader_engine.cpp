#include "pch.h"
#include "engine/shader_engine.h"
#include "engine/filesystem.h"

ShaderEngine* g_pShaderEngine = NULL;

ShaderEngine::ShaderEngine(const char* renderName)
	: m_renderName(renderName)
{
	char shadersFolder[MAX_PATH];
	sprintf(shadersFolder, "data/shaders/%s", m_renderName);
	m_shadersFolder = shadersFolder;
}

ShaderEngine::~ShaderEngine()
{
	m_shaders.clear();
}

ShaderData ShaderEngine::loadShader(const char* shaderName)
{
	ShaderData sd = { INVALID_SHADER_INDEX, INVALID_PIPELINE_INDEX };
	SHADERS_MAP::iterator it = m_shaders.find(shaderName);
	if (it == m_shaders.end())
	{
		char shaderPath[MAX_PATH];

		sprintf(shaderPath, "%s/%s.vs", m_shadersFolder.c_str(), shaderName);

		IReader* reader = g_file_system->openRead(shaderPath);
		reader->seek(End, 0);
		size_t vertex_length = reader->tell();
		reader->seek(Begin, 0);

		char* vertex_shader = (char*)g_default_allocator->allocate(vertex_length + 1, 1);
		reader->read(vertex_shader, vertex_length);
		vertex_shader[vertex_length] = '\0';

		g_file_system->deleteReader(reader);
		
		// reopen reader for fragment shader

		sprintf(shaderPath, "%s/%s.fs", m_shadersFolder.c_str(), shaderName);
		reader = g_file_system->openRead(shaderPath);
		reader->seek(End, 0);
		size_t fragment_length = reader->tell();
		reader->seek(Begin, 0);

		char* fragment_shader = (char*)g_default_allocator->allocate(fragment_length + 1, 1);
		reader->read(fragment_shader, fragment_length);
		fragment_shader[fragment_length] = '\0';

		g_file_system->deleteReader(reader);

		shaderDesc_t shader_desc = {};
		shader_desc.vertex_shader_data = vertex_shader;
		shader_desc.vertex_shader_size = vertex_length;
		shader_desc.fragment_shader_data = fragment_shader;
		shader_desc.fragment_shader_size = fragment_length;

		sd.shaderIndex = g_render_device->createShader(shader_desc);
		if (sd.shaderIndex == INVALID_SHADER_INDEX) {
			FATAL("!!! %s index is invalid", shaderName);
		}

		// free data
		g_default_allocator->deallocate(fragment_shader);
		g_default_allocator->deallocate(vertex_shader);

		pipelineDesc_t pipeline_desc = {};
		pipeline_desc.shader = sd.shaderIndex;
		pipeline_desc.layouts[0] = { VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION };
		pipeline_desc.layouts[1] = { VERTEXATTR_VEC2, SHADERSEMANTIC_TEXCOORD0 };
		pipeline_desc.layouts[2] = { VERTEXATTR_VEC2, SHADERSEMANTIC_TEXCOORD1 };
		pipeline_desc.layout_count = 3;

		sd.pipelineIndex = g_render_device->createPipeline(pipeline_desc);
		if (sd.pipelineIndex == INVALID_PIPELINE_INDEX) {
			FATAL("!!! %s pipeline is invalid", shaderName);
		}

		m_shaders.insert(std::make_pair(std::string(shaderName), sd));
	}
	else
	{
		sd = (*it).second;
	}
	return (sd);
}
