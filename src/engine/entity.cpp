#include "pch.h"
#include "engine/entity.h"

Entity::Entity()
	: m_bCanBeDestroyed(false)
{
}

Entity::~Entity() {
}

Player* Entity::castPlayer() {
	return nullptr;
}

Weapon* Entity::castWeapon() {
	return nullptr;
}
