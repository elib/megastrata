#pragma once

#include <utils.h>


#ifndef DISABLE_LIBNOISE

#include "SubGenerator.h"
#include <noise.h>

using namespace noise;

class LibNoiseLayer: public SubGenerator
{
public:

	virtual float GetRawValueAt(float x, float y, float z);
	virtual void SetVariables();

	LibNoiseLayer(int typeId);
protected:
	module::Perlin m_perlinNoise;
};

#endif