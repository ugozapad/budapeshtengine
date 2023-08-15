#include "pch.h"
#include "engine/physics_render.h"
#include "engine/debugrender.h"

PhysicsDebugDrawer g_physicsDebugDrawer;

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	g_debugRender->drawLine(
		glm::vec3(from.x()	, from.y()	, from.z()	),
		glm::vec3(to.x()	, to.y()	, to.z()	),
		glm::vec3(color.x()	, color.y()	, color.z()	)
	);
}

void PhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	drawLine(
		PointOnB,
		PointOnB + normalOnB,
		color
	);
}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString)
{
	Msg("%s", warningString);
}

void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void PhysicsDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const
{
	return m_debugMode;
}
