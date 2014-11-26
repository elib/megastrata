#pragma once

#include <utils.h>
#include "Generator.h"

class JunctionTester : public Generator
{
public:
	virtual void SetVariables();

	JunctionTester(int typeID);

	virtual void BuildCollection(Entity3dCollection *collection, WindowMapping &mapping, float height);
	virtual int GetTextureCount() { return 3;}
};