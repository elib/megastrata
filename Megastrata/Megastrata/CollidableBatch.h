#pragma once

#include <utils.h>
#include "Collidable.h"

#define MAX_BATCH_COLLIDABLES (MAX_COLLIDABLE*15)

class CollidableBatch
{
public:
	CollidableBatch(){ ClearCollidables(); }

	bool Collide(Vector3d &sourcePoint, Vector3d &velocityVector);

	void AddCollisionPlane(CollidablePlane *plane);
	void ClearCollidables();

protected:
	CollidablePlane *m_collidablePlanes[MAX_BATCH_COLLIDABLES];
	int m_collidableCount;
};