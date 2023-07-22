#ifndef IGAMEPERSISNTENT_H
#define IGAMEPERSISNTENT_H

#include "engine/object.h"

class ENGINE_API IGamePersistent : public TypedObject
{
public:
	virtual ~IGamePersistent();

	virtual void onGameStart() = 0;
};

inline IGamePersistent::~IGamePersistent()
{
}

extern ENGINE_API IGamePersistent* g_pGamePersistent;

#endif // !IGAMEPERSISNTENT_H
