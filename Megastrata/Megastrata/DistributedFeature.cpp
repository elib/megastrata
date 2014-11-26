#include "DistributedFeature.h"

#define VAR_DISTRIBUTION 0

DistributedFeature::DistributedFeature(int typeID) : Generator(typeID)
{
	spanX = spanY = spanZ = 1;
	m_targetType = RENDERED_DUMMY;
}


int DistributedFeature::GetSpanX()
{
	return spanX;
}

int DistributedFeature::GetSpanY()
{
	return spanY;
}

int DistributedFeature::GetSpanZ()
{
	return spanZ;
}

void DistributedFeature::SetDimensions(int x, int y, int z)
{
	if(x <= 0) x = -1;
	spanX = x;
	
	if(y <= 0) y = -1;
	spanY = y;
	
	if(z <= 0) z = -1;
	spanZ = z;
}

void DistributedFeature::SetEntityType(EntityRenderedType targetType)
{
	m_targetType = targetType;
}

EntityRenderedType DistributedFeature::GetEntityType()
{
	return m_targetType;
}

void DistributedFeature::BuildCollection(Entity3dCollection* collection, WindowMapping &mapping, float height)
{
	if(spanZ == -1)
		height = 0; //ignore height - it's infinite. Use generated noise from z = 0.

	//we should somehow ensure that the items are in here, even if it's bordering on the next page...

	for(int i = 0; i < mapping.GetPhysicalWidth(); i++)
	{
		for(int j = 0; j < mapping.GetPhysicalHeight(); j++)
		{
			float xpos = i, ypos = j;
			mapping.GetWorldCoordinates(xpos, ypos);
			float exists = m_layerVariables[VAR_DISTRIBUTION].GetValue(xpos, ypos, height, 0.3f);
			if(exists > 0)
			{
				if(collection->AddEntity(id, mapping, m_targetType, i, j, 0xF, spanX, spanY, spanZ))
				{
					collection->SetEntityVariable(i, j, 0, 1.0f);
				}
			}
		}
	}
}

void DistributedFeature::SetVariables()
{
	m_layerVariables[VAR_DISTRIBUTION].Initialize(this, "Distribution", 0.5f, 1.0f, 0);
	m_variableCount = 1;
}

int DistributedFeature::GetTextureCount()
{
	switch(m_targetType)
	{
	case RENDERED_SHAFT:
		return 3;
	default:
		return 1;
	}
}