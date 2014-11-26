#pragma once

#include "SubGenerator.h"

class SquareOverlay : public SubGenerator
{
public:
	virtual float GetRawValueAt(float x, float y, float z);
	SquareOverlay(int type_id);
private:
	bool inSquare(float x, float y);
	virtual void SetVariables();

};