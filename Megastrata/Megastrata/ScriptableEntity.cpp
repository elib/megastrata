#include "ScriptableEntity.h"

#include "World.h"
#include "VehicleEventRegistry.h"

ScriptableEntity::ScriptableEntity(void)
{
	alive = true;
	visible = false;
	collection_id = NULL;
}

ScriptableEntity::~ScriptableEntity(void)
{
	alive = false;
}