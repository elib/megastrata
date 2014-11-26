#include "CollidableCollection.h"

#include "CollidableBatch.h"

#include "World.h"

CollidableCollection::CollidableCollection(void)
{
	ClearCollidables();
}


void CollidableCollection::ClearCollidables()
{
	m_collidableCount = 0;
}

void CollidableCollection::AddCollisionPlane(float *currentMatrix, Vector3d ver1, Vector3d ver2, Vector3d ver3, Vector3d ver4)
{
	if(m_collidableCount >= MAX_COLLIDABLE)
		return;

	//transform vertices
	Vector3d v1 = ver1.Transform(currentMatrix);
	Vector3d v2 = ver2.Transform(currentMatrix);
	Vector3d v3 = ver3.Transform(currentMatrix);
	Vector3d v4 = ver4.Transform(currentMatrix);

	m_collidablePlanes[m_collidableCount++].Initialize(v1, v2, v3, v4);
}

void CollidableCollection::AddCollidablesToBatch(CollidableBatch *batch)
{
	for(int i = 0; i < m_collidableCount; i++)
	{
		batch->AddCollisionPlane(&m_collidablePlanes[i]);
	}
}