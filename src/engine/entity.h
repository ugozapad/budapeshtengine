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

	inline bool canBeDestroyed() const { return (m_bCanBeDestroyed); }
	inline void	setCanBeDestroyed() { m_bCanBeDestroyed = true; }

	void setPosition(const glm::vec3& pos);
	glm::vec3 getPosition();

	void setRotation(const glm::vec3& rot);
	glm::vec3 getRotation();

	void setScale(const glm::vec3& scale);
	glm::vec3 getScale();

	glm::mat4 getTranslationMatrix();

protected:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
  
private:
	bool m_bCanBeDestroyed;
  
};

#endif