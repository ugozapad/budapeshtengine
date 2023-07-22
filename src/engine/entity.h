#ifndef ENTITY_H
#define ENTITY_H

#include "engine/object.h"

// Forward declaration
class Player;
class Weapon;

//! Base entity class.
class ENGINE_API Entity : public TypedObject {
public:
	OBJECT_IMPLEMENT(Entity, TypedObject);

	Entity();
	virtual ~Entity();

	virtual void update(float dt);

	// Inplace cast'ers
	virtual Player* castPlayer();
	virtual Weapon* castWeapon();

protected:
	glm::vec3 m_position;
};

#endif