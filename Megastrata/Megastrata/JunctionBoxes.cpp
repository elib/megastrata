#include "JunctionBoxes.h"

#define BOX_CONNECTIVITY 0
#define BOX_EXISTS 1
#define BOX_HEIGHT_VAR 2
#define BOX_BEVEL_VAR 3

#define MIN_BOX_WIDTH 0.3f

#define CONNECTOR_WIDTH (GRID_SIZE * 0.1f)

JunctionBoxes::JunctionBoxes(int typeID) : Generator(typeID)
{
}

void JunctionBoxes::SetVariables()
{
	m_layerVariables[BOX_CONNECTIVITY].Initialize(this, "Connectivity", 0.0f, 1.1f, 0.0f);
	m_layerVariables[BOX_EXISTS].Initialize(this, "Node exists", 0.5, 1.0f, 0);
	m_layerVariables[BOX_HEIGHT_VAR].Initialize(this, "Node height", MIN_BOX_WIDTH, 2.0f, MIN_BOX_WIDTH);
	m_layerVariables[BOX_BEVEL_VAR].Initialize(this, "Bevel info", 0, 1.1f, 0.0f);

	m_variableCount = 4;
}

void JunctionBoxes::BuildCollection(Entity3dCollection *collection, WindowMapping &mapping, float height)
{
	int size_x = mapping.GetPhysicalWidth(), size_y = mapping.GetPhysicalHeight();

	//iterating over space
	for(int j = 0; j < size_y; j++)
	{
		for(int i = 0; i < size_x; i++)	
		{
			float xpos = i, ypos = j;
			mapping.GetWorldCoordinates(xpos, ypos);

			float boxExists = m_layerVariables[BOX_EXISTS].GetValue(xpos, ypos, height, 0.3f);

			if(boxExists > 0.0f)
			{
				float boxHeight = m_layerVariables[BOX_HEIGHT_VAR].GetValue(xpos, ypos, height);
				//draw this cell

				float value = (m_layerVariables[BOX_CONNECTIVITY].GetValue(xpos, ypos, height, 0.5f));

				if (value > 1.0f) value = 1.0; //CLARMP
				if (value < 0.0f) value = 0.0; //CLARMP

				unsigned char connections = (unsigned char) 0xF * value;
				if(collection->AddEntity(id, mapping, RENDERED_JUNCTIONBOXES, i, j, connections))
				{
					collection->SetEntityVariable(i, j, 0, m_layerVariables[BOX_HEIGHT_VAR].GetValue(xpos, ypos, height));
					collection->SetEntityVariable(i, j, 1, m_layerVariables[BOX_BEVEL_VAR].GetValue(xpos, ypos, height, 0.1f));
				}
			}
		}
	}
}