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

	// Inplace cast'ers
	virtual Player* castPlayer();
	virtual Weapon* castWeapon();

	inline bool canBeDestroyed() const { return (m_bCanBeDestroyed); }
	inline void	setCanBeDestroyed() { m_bCanBeDestroyed = true; }

private:
	bool m_bCanBeDestroyed;
};

#endif