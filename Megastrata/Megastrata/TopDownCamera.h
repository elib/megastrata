#pragma once

#include "Camera.h"
#include "CharacterObserver.h"

class TopDownCamera: public Camera, public CharacterObserver
{
public:
	virtual void OnCharacterMove(Vector3d position);
};