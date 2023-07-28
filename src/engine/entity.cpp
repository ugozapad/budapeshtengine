#include "pch.h"
#include "engine/entity.h"

Entity::Entity() :
	m_position(0.0f), m_bCanBeDestroyed(false)
{
}

Entity::~Entity() {
}

void Entity::update(float dt) {
}

Player* Entity::castPlayer() {
	return nullptr;
}

Weapon* Entity::castWeapon() {
	return nullptr;
}
