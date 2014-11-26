#pragma once

#include "VehicleObserver.h"

typedef vector<VehicleObserver*> ObserverArray;
typedef ObserverArray::iterator ObserverIterator;


class VehicleEventRegistry
{
public:

	void VehicleArriveStation(Vehicle *vehicle);
	void VehicleDepartStation(Vehicle *vehicle);
	void VehicleMove(Vehicle *vehicle);

	void AddObserver(VehicleObserver *Observer);
private:
	 ObserverArray m_Observers;
};