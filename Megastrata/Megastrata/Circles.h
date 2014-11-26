#pragma once


#include "SubGenerator.h"


class Circles : public SubGenerator
{
public:
	virtual float GetRawValueAt(float x, float y, float z);
	virtual void SetVariables();
	Circles(int typeID);

private:
	bool inCircle(float x, float y);
	
};