#ifndef ENTITY_H
#define ENTITY_H

#include "engine/object.h"

// Forward declaration
class Player;
class Weapon;

//! Base entity class.
class Entity : public ITypedObject {
public:
	OBJECT_IMPLEMENT(Entity, ITypedObject);

	Entity();
	virtual ~Entity();

	// Inplace cast'ers
	virtual Player* castPlayer();
	virtual Weapon* castWeapon();
};

#endif