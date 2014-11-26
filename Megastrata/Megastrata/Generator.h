#pragma once

#include <utils.h>
#include "GeneratorBase.h"
#include "Entity3dCollection.h"

class World;

class Generator : public GeneratorBase
{
public:
	Generator(int typeID);

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);

	virtual void BuildCollection(Entity3dCollection* collection, WindowMapping &mapping, float height) = 0;
	virtual int GetTextureCount() = 0;
};