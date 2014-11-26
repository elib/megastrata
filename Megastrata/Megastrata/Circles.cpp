#include "Circles.h"

#include "WindowMapping.h"

#define CIRCLE_RADIUS 0

Circles::Circles(int typeID) : SubGenerator(typeID)
{
}


float Circles::GetRawValueAt(float x, float y, float z)
{
	float gray;
	if(inCircle(x,y))
		gray = 1.0f;
	else
		gray = 0.0f;

	return gray;
}

void Circles::SetVariables()
{
	m_layerVariables[CIRCLE_RADIUS].Initialize(this, "Radius", 0.2f, 0.49f, 0.01f);
	m_variableCount = 1;
}

bool Circles::inCircle(float x, float y)
{
	float circleRadius;

	float grid_x, grid_y;
	grid_x = GRID_SIZE;
	grid_y = GRID_SIZE;

	//get a fixed point in the cell where this circle should be
	float cell_x, cell_y;
	cell_x = grid_x * floor(x / grid_x) * m_transform.Scale.x;
	cell_y = grid_y * floor(y / grid_y) * m_transform.Scale.y;

	circleRadius = m_layerVariables[CIRCLE_RADIUS].GetValue(cell_x, cell_y, 0.2);

	//get position inside of grid's circle
	float x_off = (x - (grid_x * floor(x / grid_x))) - grid_x/2;
	float y_off = (y - (grid_y * floor(y / grid_y))) - grid_y/2;

	if((x_off) * (x_off) + (y_off) * (y_off) < circleRadius * circleRadius)
		return true;

	return false;
}