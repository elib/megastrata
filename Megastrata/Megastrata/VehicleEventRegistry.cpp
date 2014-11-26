#include "VehicleEventRegistry.h"



void VehicleEventRegistry::AddObserver(VehicleObserver *Observer)
{
	m_Observers.push_back(Observer);
}

void VehicleEventRegistry::VehicleArriveStation(Vehicle *vehicle)
{
	ObserverIterator it = m_Observers.begin();
	while(it != m_Observers.end())
	{
		(*it)->OnVehicleArriveStation(vehicle);
		it++;
	}
}

void VehicleEventRegistry::VehicleDepartStation(Vehicle *vehicle)
{
	ObserverIterator it = m_Observers.begin();
	while(it != m_Observers.end())
	{
		(*it)->OnVehicleDepartStation(vehicle);
		it++;
	}
}

void VehicleEventRegistry::VehicleMove(Vehicle *vehicle)
{
	ObserverIterator it = m_Observers.begin();
	while(it != m_Observers.end())
	{
		(*it)->OnVehicleMove(vehicle);
		it++;
	}
}