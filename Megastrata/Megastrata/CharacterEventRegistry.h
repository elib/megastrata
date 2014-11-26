#pragma once

#include "CharacterObserver.h"

typedef vector<CharacterObserver*> CharacterObserverArray;
typedef CharacterObserverArray::iterator CharacterObserverIterator;

class CharacterEventRegistry
{
public:
	void CharacterMove(Vector3d Position);

	void AddObserver(CharacterObserver *Observer);
private:
	 CharacterObserverArray m_Observers;
};