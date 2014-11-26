#pragma once

#include "Character.h"
#include <vector>

class CharacterObserver
{
public:
	virtual ~CharacterObserver(void);

	virtual void OnCharacterMove(Vector3d position) {}
};