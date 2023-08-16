#include "pch.h"
#include "engine/entity.h"

Entity::Entity() :
	m_position(0.0f), m_rotation(0.0f), m_scale(1.0f),
	m_bCanBeDestroyed(false)
{
}

Entity::~Entity() {
}

void Entity::Update(float dt) {
}

Player* Entity::CastPlayer() {
	return nullptr;
}

Weapon* Entity::CastWeapon() {
	return nullptr;
}

void Entity::SetPosition(const glm::vec3& pos)
{
	m_position = pos;
}

glm::vec3 Entity::GetPosition()
{
	return m_position;
}

void Entity::SetRotation(const glm::vec3& rot)
{
	m_rotation = rot;
}

glm::vec3 Entity::GetRotation()
{
	return m_rotation;
}

void Entity::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}

glm::vec3 Entity::GetScale()
{
	return m_scale;
}

// Kirill: #TODO update matrix if it's dirty
glm::mat4 Entity::GetTranslationMatrix()
{
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, m_position);

	// rotation
	trans = glm::rotate(trans, m_rotation.x, glm::vec3(1.0, 0.0, 0.0));
	trans = glm::rotate(trans, m_rotation.y, glm::vec3(0.0, 1.0, 0.0));
	trans = glm::rotate(trans, m_rotation.z, glm::vec3(0.0, 0.0, 1.0));

	// scale
	trans = glm::scale(trans, m_scale);

	return trans;
}
