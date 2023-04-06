#ifndef ENTITY_H
#define ENTITY_H

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
class Entity : public IPhysicsObject {
public:
	Entity();
	virtual ~Entity();

	// Inplace cast'ers
	virtual Player* castPlayer();
	virtual Weapon* castWeapon();
};

#endif