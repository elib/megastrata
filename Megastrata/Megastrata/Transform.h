#pragma once

#include <utils.h>
#include "Serializable.h"


class Transform: public Serializable
{
public:
	Transform(void);

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);

	POINT_FLOAT Translation;
	POINT_FLOAT Scale;

	void Apply(float &x, float &y);

private:
};