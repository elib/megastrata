#pragma once
#include <utils.h>
#include "Generator.h"

class Boxes : public Generator
{
public:
	virtual void SetVariables();

	Boxes(int typeID);

protected:
	virtual void BuildCollection(Entity3dCollection *collection, WindowMapping &mapping, float height);
	virtual int GetTextureCount() { return 3;}
};