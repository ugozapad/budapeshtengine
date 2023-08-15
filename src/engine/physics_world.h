#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "engine/varmanager.h"

class PhysicsWorld
{
public:
	static void staticInit();

public:
	PhysicsWorld();
	~PhysicsWorld();

	void create();
	void destroy();

	void update(float deltaTime);

	btDynamicsWorld* getWorld();

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDynamicsWorld* m_dynamicsWorld;

};

#endif // !PHYSICS_WORLD_H
