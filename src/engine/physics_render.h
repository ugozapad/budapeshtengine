#ifndef PHYSICSDEBUGDRAW_H
#define PHYSICSDEBUGDRAW_H

#include "LinearMath/btIDebugDraw.h"

class PhysicsDebugDrawer : public btIDebugDraw
{
public:
	//void init();
	//void shutdown();

	// btIDebugDraw inherit
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	void reportErrorWarning(const char* warningString) override;
	void draw3dText(const btVector3& location, const char* textString) override;
	void setDebugMode(int debugMode) override;
	int getDebugMode() const override;

private:
	int m_debugMode;
};

extern PhysicsDebugDrawer g_physicsDebugDrawer;

#endif