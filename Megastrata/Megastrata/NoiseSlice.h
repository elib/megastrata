#pragma once
#include <utils.h>

#include "SubGenerator.h"

class NoiseSlice : public SubGenerator
{
public:
	NoiseSlice(int type_id);
	virtual float GetRawValueAt(float x, float y, float z);

private:

	void UpdateWindowMapper();
	virtual void SetVariables();
};