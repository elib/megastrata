#include "Boxes.h"
#include "Camera.h"

#define BOX_WIDTH_VAR 0
#define BOX_HEIGHT_VAR 1

Boxes::Boxes(int typeID) : Generator(typeID)
{
}

void Boxes::SetVariables()
{
	m_layerVariables[BOX_WIDTH_VAR].Initialize(this, "Box width", 0.2f, GRID_SIZE - 0.1, 0.1f);
	m_layerVariables[BOX_HEIGHT_VAR].Initialize(this, "Box height", 0.2f, 2.0f, 0.2f);
	m_variableCount = 2;
}

void Boxes::BuildCollection(Entity3dCollection *collection, WindowMapping &mapping, float height)
{
	int size_x = mapping.GetPhysicalWidth(), size_y = mapping.GetPhysicalHeight();

	//iterating over space
	for(int j = 0; j < size_y; j++)
	{
		for(int i = 0; i < size_x; i++)	
		{
			float xpos =i, ypos = j;
			mapping.GetWorldCoordinates(xpos, ypos);
			float boxWidth = m_layerVariables[BOX_WIDTH_VAR].GetValue(xpos, ypos, height, 0.3);
			float boxHeight = m_layerVariables[BOX_HEIGHT_VAR].GetValue(xpos, ypos, height);

			if(boxWidth > 0 && boxHeight > 0)
			{
				collection->AddEntity(id, mapping, RENDERED_JUNCTIONBOXES, i, j);
			}
		}
	}
}