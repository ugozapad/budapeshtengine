#ifndef GAMEPERSISTENT_H
#define GAMEPERSISTENT_H

#include "engine/igamepersistent.h"

class GamePersistent : public IGamePersistent
{
public:
	OBJECT_IMPLEMENT(GamePersistent, TypedObject);

	GamePersistent();
	~GamePersistent();

	void onGameStart() override;
};

#endif // !GAMEPERSISTENT_H
