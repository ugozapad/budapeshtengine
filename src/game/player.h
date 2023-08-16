#ifndef PLAYER_H
#define PLAYER_H

#include "engine/entity.h"
#include "engine/camera.h"

class Player : public Entity {
public:
	OBJECT_IMPLEMENT(Player, Entity);

	Player();
	~Player();

	void Update(float dt) override;

	Player* CastPlayer() override { return this; }

	void activateCamera();

private:
	Camera m_camera;

};

#endif // !PLAYER_H
