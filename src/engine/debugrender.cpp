#include "pch.h"
#include "debugrender.h"
#include "varmanager.h"
#include "engine.h"
#include "shader_engine.h"
#include "camera.h"
#include "material_system.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Var g_varDebugDraw("r_draw_dbg", "1", "Debug stuff drawing", VARFLAG_NOSAVE);

ENGINE_API DebugRender* g_debugRender;

DebugRender::DebugRender()
{
	m_verticesBuffer = INVALID_BUFFER_INDEX;
	m_shaderProgram = INVALID_PIPELINE_INDEX;
}

DebugRender::~DebugRender()
{
	m_verticesBuffer = INVALID_BUFFER_INDEX;
	m_shaderProgram = INVALID_PIPELINE_INDEX;
}

void DebugRender::initialize()
{
	g_VarManager.RegisterVar(&g_varDebugDraw);

	float points[12];
	memset(points, 0, sizeof(points));

	bufferDesc_t buffer_desc = {};
	buffer_desc.type = BUFFERTYPE_VERTEX;
	buffer_desc.access = BUFFERACCESS_DYNAMIC;
	buffer_desc.size = sizeof(points);

	m_verticesBuffer = g_engine->GetRenderDevice()->createBuffer(buffer_desc);

	ShaderData shader = g_pShaderEngine->LoadShader("debug_draw");
	m_shaderProgram = shader.pipelineIndex;
}

void DebugRender::shutdown()
{
	if (m_verticesBuffer != INVALID_BUFFER_INDEX) {
		g_engine->GetRenderDevice()->deleteBuffer(m_verticesBuffer);
	}
}

void DebugRender::drawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color)
{
	if (g_varDebugDraw.GetIntValue() < 1)
		return;

	Line line;
	line.from = from;
	line.color0 = color;
	line.to = to;
	line.color1 = color;
	m_lines.push_back(line);
}

void DebugRender::renderFrame()
{
	if (g_varDebugDraw.GetIntValue() < 1)
		return;

	beginDraw();

	// draw lines
	drawLinesInternal();

	// and clear them
	m_lines.clear();

	endDraw();
}

void DebugRender::drawAxis(const glm::vec3& vec)
{
	const float length = 0.2f;
	drawLine(vec, glm::vec3(vec.x + length, vec.y, vec.z), glm::vec3(1.0f, 0.0, 0.0f));
	drawLine(vec, glm::vec3(vec.x, vec.y + length, vec.z), glm::vec3(0.0f, 0.0f, 1.0f));
	drawLine(vec, glm::vec3(vec.x, vec.y, vec.z + length), glm::vec3(0.0f, 1.0f, 0.0f));
}

void DebugRender::beginDraw()
{
}

void DebugRender::endDraw()
{
}

void DebugRender::drawLinesInternal()
{
	if (m_lines.empty())
		return;

	static int last_capacity = 0;

	IRenderDevice* pRenderDevice = g_engine->GetRenderDevice();

	// grow buffer is that is nessecary
	if (last_capacity < m_lines.size()) {

		pRenderDevice->deleteBuffer(m_verticesBuffer);

		bufferDesc_t buffer_desc = {};
		buffer_desc.type = BUFFERTYPE_VERTEX;
		buffer_desc.access = BUFFERACCESS_DYNAMIC;
		buffer_desc.size = m_lines.size() * sizeof(Line);

		m_verticesBuffer = pRenderDevice->createBuffer(buffer_desc);
	}

	pRenderDevice->updateBuffer(m_verticesBuffer, m_lines.data(), m_lines.size() * sizeof(Line));

	// bind our shader
	pRenderDevice->setPipeline(m_shaderProgram);

	// #TODO: Fix stupid bug, when we get very far from wireframe and lines can start cliping
	//View view = g_render->topView();
	//view.proj[2][3] -= 0.0001f;

	viewport_t viewport = g_engine->GetViewport();
	float aspectRatio = (float)viewport.width / (float)viewport.height;

	// calculate projection matrix
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(75.0f), aspectRatio, 0.1f, 1000.0f);
	proj[2][3] -= 0.0001f; // #TODO: Fix stupid bug, when we get very far from wireframe and lines can start cliping

	Camera* camera = g_CameraManager.GetActiveCamera();

	glm::mat4 mvp = glm::identity<glm::mat4>();
	mvp = proj * (camera ? camera->GetViewMatrix() : glm::identity<glm::mat4>()) * glm::identity<glm::mat4>();
	pRenderDevice->setVSConstant(0, &mvp[0], MATRIX4_SIZE);

	// BINDINGS
	pRenderDevice->beginBinding();
	pRenderDevice->setVertexBuffer(m_verticesBuffer);

	// SHUT THE FUCK UP
	pRenderDevice->setTexture(0, g_material_system.GetNoTexture()->GetTextureIndex());
	pRenderDevice->setTexture(1, g_material_system.GetNoTexture()->GetTextureIndex());

	pRenderDevice->endBinding();

	// disable write to depth
	//g_renderDevice->setDepthTest(false);
	//g_renderDevice->setDepthWrite(false);

	// size of line
	//glLineWidth(2.0f);

	// draw stuff
	pRenderDevice->draw(0, m_lines.size() * 2, 1);

	last_capacity = m_lines.size();
}
