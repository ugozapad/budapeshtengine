#ifndef ENTITY_H
#define ENTITY_H

#include "engine/object.h"

// Forward declaration
class Player;
class Weapon;

//! Physics object interface
class IPhysicsObject {
public:
	virtual ~IPhysicsObject() {}

	//virtual void onCollisionEnter(IPhysicsObject* a, IPhysicsObject* b) = 0;
	//virtual void onCollisionExit(IPhysicsObject* a, IPhysicsObject* b) = 0;
};

//! Base entity class.
class Entity : public ITypedObject, public IPhysicsObject {
public:
	OBJECT_IMPLEMENT(Entity, ITypedObject);

	Entity();
	virtual ~Entity();

	// Inplace cast'ers
	virtual Player* castPlayer();
	virtual Weapon* castWeapon();
};

class Player : public Entity {
public:
	OBJECT_IMPLEMENT(Player, Entity);

	Player();
	~Player();

	Player* castPlayer() override { return this; }
};

#endif