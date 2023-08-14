#ifndef PLAYER_H
#define PLAYER_H

#include "engine/entity.h"

class Player : public Entity {
public:
	OBJECT_IMPLEMENT(Player, Entity);

	Player();
	~Player();

	void update(float dt) override;

	Player* castPlayer() override { return this; }
};

#endif // !PLAYER_H
