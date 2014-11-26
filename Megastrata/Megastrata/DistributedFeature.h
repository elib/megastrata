#pragma once
#include <utils.h>
#include "Generator.h"
#include "Entity3dCollection.h"

class DistributedFeature: public Generator
{
public:
	DistributedFeature(int typeID);

	int GetSpanX();
	int GetSpanY();
	int GetSpanZ();
	void SetDimensions(int x, int y, int z);

	void SetEntityType(EntityRenderedType targetType);
	EntityRenderedType GetEntityType();

	virtual void BuildCollection(Entity3dCollection* collection, WindowMapping &mapping, float height);
	virtual int GetTextureCount();

protected:
	void SetVariables();
	//specify the dimensions of the feature in normal gridmarks
	//must be positive and odd, or -1 (infinite)
	int spanX, spanY, spanZ;
	EntityRenderedType m_targetType;
};