#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "engine/varmanager.h"

class PhysicsWorld
{
public:
	static void StaticInit();

public:
	PhysicsWorld();
	~PhysicsWorld();

	void Create();
	void Destroy();

	void Update(float deltaTime);

	btDynamicsWorld* GetWorld();

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDynamicsWorld* m_dynamicsWorld;

};

#endif // !PHYSICS_WORLD_H
