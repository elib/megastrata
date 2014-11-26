#include <simplexnoise1234.h>
#include "NoiseSlice.h"

#include "Viewer2d.h"
#include "GeneratorBase.h"

#define VAR_CUTOFF  0

NoiseSlice::NoiseSlice(int type_id) : SubGenerator(type_id)
{
}

void NoiseSlice::SetVariables()
{
	m_layerVariables[VAR_CUTOFF].Initialize(this, "Cutoff", 0.5f);

	//lame:
	m_variableCount = 1;
}


float NoiseSlice::GetRawValueAt(float x, float y, float z)
{
	float noise = SimplexNoise1234::noise( x, y, z );

	float cutoff = m_layerVariables[VAR_CUTOFF].GetValue(x, y, z);
	float scalefactor = 0;

	float res = (noise + 1.0f)*0.5f;
	if(res > cutoff)
	{
		scalefactor = (res - cutoff) * (1/(1 - cutoff));
		scalefactor = MIN(scalefactor, 1.0f);
		scalefactor = MAX(scalefactor, 0.0f);
	}

	return scalefactor;
}