#include "LibNoiseLayer.h"

#ifndef DISABLE_LIBNOISE

LibNoiseLayer::LibNoiseLayer(int typeId):SubGenerator(typeId)
{
	//m_perlinNoise.SetOctaveCount(3);
	m_perlinNoise.SetFrequency(0.1);
}

float LibNoiseLayer::GetRawValueAt(float x, float y, float z)
{
	double val = m_perlinNoise.GetValue(x, y, 0);
	val += 1.7F;
	val /= 3.4F;

	val = MIN(1.0F, val);
	val = MAX(0.0F, val);

	//if(val < 0)	{LOG("Value small: %g", val);}

	//if(val > 1.0F) {LOG("Value big: %g", val);}
	return (float)val;
}

void LibNoiseLayer::SetVariables()
{
}

#endif