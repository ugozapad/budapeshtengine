#ifndef SHADER_ENGINE_H
#define SHADER_ENGINE_H

#include "engine/allocator.h"
#include "engine/array.h"
#include "engine/entity.h"

#include "render/irenderdevice.h"

#include <Windows.h>
#include <map>
#include <string>

struct ShaderData // 8 bytes, it fits in u64, so we can return it just by value
{
	shaderIndex_t	shaderIndex;
	pipelineIndex_t pipelineIndex;
};

class ShaderEngine
{
	typedef std::map<std::string, ShaderData> SHADERS_MAP;
public:
	ShaderEngine(const char* renderName);
	~ShaderEngine();

	ShaderData loadShader(const char* shaderName);

private:
	const char* m_renderName;
	std::string m_shadersFolder;
	SHADERS_MAP m_shaders;
};

extern ShaderEngine* g_pShaderEngine;

#endif
