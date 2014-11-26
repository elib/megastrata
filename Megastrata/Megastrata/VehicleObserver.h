#pragma once

#include "Vehicle.h"
#include <vector>

class VehicleObserver
{
public:
	virtual ~VehicleObserver(void);

	virtual void OnVehicleArriveStation(Vehicle *vehicle){}
	virtual void OnVehicleDepartStation(Vehicle *vehicle){}
	virtual void OnVehicleMove(Vehicle *vehicle){}

private:
	
};