#include "CharacterEventRegistry.h"

void CharacterEventRegistry::AddObserver(CharacterObserver *Observer)
{
	m_Observers.push_back(Observer);
}

void CharacterEventRegistry::CharacterMove(Vector3d position)
{
	CharacterObserverIterator it = m_Observers.begin();
	while(it != m_Observers.end())
	{
		(*it)->OnCharacterMove(position);
		it++;
	}
}