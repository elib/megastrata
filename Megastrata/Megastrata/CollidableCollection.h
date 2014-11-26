#pragma once

#include "Collidable.h"
#include "CollidableBatch.h"

class CollidableCollection
{
public:
	CollidableCollection(void);

	void ClearCollidables();
	void AddCollisionPlane(float *currentMatrix, Vector3d ver1, Vector3d ver2, Vector3d ver3, Vector3d ver4);

	void AddCollidablesToBatch(CollidableBatch *batch);

protected:

	CollidablePlane m_collidablePlanes[MAX_COLLIDABLE];
	int m_collidableCount;
};