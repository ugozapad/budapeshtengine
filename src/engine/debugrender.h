#ifndef DEBUGRENDER_H
#define DEBUGRENDER_H

#include "render/irenderdevice.h"

// Ported from old engine iteration 
class ENGINE_API DebugRender
{
public:
	DebugRender();
	~DebugRender();

	void initialize();
	void shutdown();

	void drawAxis(const glm::vec3& vec);
	void drawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);

	void renderFrame();

private:
	void beginDraw();
	void endDraw();

	void drawLinesInternal();

private:
	// Primitives
	struct Line
	{
		glm::vec3 from;
		glm::vec3 color0;

		glm::vec3 to;
		glm::vec3 color1;
	};

	Array<Line> m_lines;

private:
	bufferIndex_t m_verticesBuffer;
	pipelineIndex_t m_shaderProgram;
};

extern ENGINE_API DebugRender* g_debugRender;

#endif // !DEBUGRENDER_H
