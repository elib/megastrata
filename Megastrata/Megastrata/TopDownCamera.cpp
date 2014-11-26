#include "TopDownCamera.h"
#include "World.h"

void TopDownCamera::OnCharacterMove(Vector3d charpos)
{
	//make topdown camera follow the character.
	Vector3d diff = charpos - Lookat;
	Lookat = Lookat + diff;
	Position = Position + diff;
}