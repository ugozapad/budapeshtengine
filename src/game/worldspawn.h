#ifndef WORLDSPAWN_H
#define WORLDSPAWN_H

#include "engine/entity.h"

class WorldSpawn : public Entity
{
public:
	OBJECT_IMPLEMENT(WorldSpawn, Entity);

	WorldSpawn();
	~WorldSpawn();
};

#endif // !WORLDSPAWN_H
