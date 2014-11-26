#pragma once

#include <utils.h>
#include "Vector3d.h"

#include "CollidableBatch.h"
#include "CollidableCollection.h"

#define MAX_SCRIPTABLE		100

class ScriptableEntity
{
public:
	virtual void Render(){}
	virtual void Update(ElapsedTime absoluteTickCount){}

	virtual void GetCollidables(CollidableBatch *batch){}

	ScriptableEntity(void);
	virtual ~ScriptableEntity(void);

	int collection_id;

protected:
	bool alive;
	
	virtual void UpdateCollidables(){}

	bool visible;

	CollidableCollection m_collidables;
};