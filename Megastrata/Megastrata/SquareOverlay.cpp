#include "SquareOverlay.h"

#define SQUARE_WIDTH_VAR 0

SquareOverlay::SquareOverlay(int type_id) : SubGenerator(type_id)
{
}


bool SquareOverlay::inSquare(float x, float y)
{

	//square and spacing fits in unit width/height.
	//for now, squares remain squares. No stretching on any axis.

	float squareWidth;

	float grid_x, grid_y;
	grid_x = GRID_SIZE;
	grid_y = GRID_SIZE;

	////get a fixed point in the cell where this square should be
	float cell_x, cell_y;
	cell_x = grid_x * floor(x / grid_x) * m_transform.Scale.x;
	cell_y = grid_y * floor(y / grid_y) * m_transform.Scale.y;

	squareWidth = m_layerVariables[SQUARE_WIDTH_VAR].GetValue(cell_x, cell_y, 0.2);

	//get position inside of grid square
	float x_off = (x - (grid_x * floor(x / grid_x))) - grid_x/2;
	float y_off = (y - (grid_y * floor(y / grid_y))) - grid_y/2;

	if((x_off > (-squareWidth/2)) && (y_off > (-squareWidth/2)) && (x_off < squareWidth/2) && (y_off < squareWidth/2))
		return true;

	return false;
}

float SquareOverlay::GetRawValueAt(float x, float y, float z)
{
	float gray;

	if(inSquare(x, y))
		gray = 1.0f;
	else
		gray = 0.0f;

	return gray;
}


void SquareOverlay::SetVariables()
{
	m_layerVariables[SQUARE_WIDTH_VAR].Initialize(this, "Width", 0.2f, GRID_SIZE - 0.01f, 0.01f);
	m_variableCount = 1;
}