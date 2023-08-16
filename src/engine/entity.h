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

	virtual void Update(float dt);

	// Inplace cast'ers
	virtual Player* CastPlayer();
	virtual Weapon* CastWeapon();

	inline bool CanBeDestroyed() const { return (m_bCanBeDestroyed); }
	inline void	SetCanBeDestroyed() { m_bCanBeDestroyed = true; }

	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition();

	void SetRotation(const glm::vec3& rot);
	glm::vec3 GetRotation();

	void SetScale(const glm::vec3& scale);
	glm::vec3 GetScale();

	glm::mat4 GetTranslationMatrix();

protected:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
  
private:
	bool m_bCanBeDestroyed;
  
};

#endif