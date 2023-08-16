#include "pch.h"
#include "physics_world.h"
#include "physics_render.h"

static Var g_phFixedUpdate("ph_fixed_update", "0.016666", "Fixed update rate value (ms)", VARFLAG_SERVER_PROTECT | VARFLAG_CHEAT);

static void* bulletAlignedAllocFunc(size_t size, int alignment)
{
	return g_allocator->Allocate(size, alignment);
}

static void  bulletAlignedFreeFunc(void* memblock)
{
	g_allocator->Deallocate(memblock);
}

static void* bulletAllocFunc(size_t size)
{
}

static void  bulletFreeFunc(void* memblock)
{
}

void PhysicsWorld::StaticInit()
{
	g_VarManager.RegisterVar(&g_phFixedUpdate);

	btAlignedAllocSetCustomAligned(&bulletAlignedAllocFunc, &bulletAlignedFreeFunc);
}

PhysicsWorld::PhysicsWorld()
{
	m_collisionConfiguration = nullptr;
	m_dispatcher = nullptr;
	m_overlappingPairCache = nullptr;
	m_solver = nullptr;
	m_dynamicsWorld = nullptr;
}

PhysicsWorld::~PhysicsWorld()
{
}

void PhysicsWorld::Create()
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(
		m_dispatcher,
		m_overlappingPairCache,
		m_solver,
		m_collisionConfiguration
	);

	m_dynamicsWorld->setDebugDrawer(&g_physicsDebugDrawer);
}

void PhysicsWorld::Destroy()
{
	if (m_dynamicsWorld) {
		delete m_dynamicsWorld;
		m_dynamicsWorld = nullptr;
	}

	if (m_solver) {
		delete m_solver;
		m_solver = nullptr;
	}

	if (m_overlappingPairCache) {
		delete m_overlappingPairCache;
		m_overlappingPairCache = nullptr;
	}

	if (m_dispatcher) {
		delete m_dispatcher;
		m_dispatcher = nullptr;
	}

	if (m_collisionConfiguration) {
		delete m_collisionConfiguration;
		m_collisionConfiguration = nullptr;
	}
}

void PhysicsWorld::Update(float deltaTime)
{
	if (m_dynamicsWorld) {
		m_dynamicsWorld->stepSimulation(deltaTime, 1, g_phFixedUpdate.GetFloatValue());
	}
}

btDynamicsWorld* PhysicsWorld::GetWorld()
{
	return m_dynamicsWorld;
}
