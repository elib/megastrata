#pragma once

#include "ScriptableEntity.h"
#include "Sphere.h"

class Vehicle: public ScriptableEntity
{
public:

	virtual void Render();
	virtual void Update(ElapsedTime absoluteTickCount);

	virtual void GetCollidables(CollidableBatch *batch);

	void Initialize(Vector3d targetpos, Vector3d targetdimensions, float targetorientation);

	bool InVehicle(Sphere point);

	Vehicle(void);

	const Vector3d GetPosition();
	Sphere m_boundingSphere;

	bool InStation();

protected:

	virtual void UpdateCollidables();

	void NotifyChangedPosition();

	Vector3d position;
	Vector3d dimensions;
	float orientation;
	
	float height_delta;
	void DetermineIfInStation(float zdistance);
	bool is_in_station;

};