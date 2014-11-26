#pragma once
#include <utils.h>
#include "Camera.h"

#include "VehicleObserver.h"

#include "Sphere.h"

class Character: public Camera, public VehicleObserver
{
public:

	Character(void);

	virtual void OnVehicleArriveStation(Vehicle *vehicle);
	virtual void OnVehicleDepartStation(Vehicle *vehicle);
	virtual void OnVehicleMove(Vehicle *vehicle);

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);
	string GetLookDirection();

	virtual void Reset();

	void Go(int forward, int right, int up, bool snap, int ms);
	//void Strafe(int ms);
	void ChangeLook(float dx, float dy);

	void RenderDirectionIndicator(float height);

private:

	void NotifyChangedPosition();
	void LeaveElevator();

	void InitializeTexture();
	float m_turnangle;
	float m_upangle;
	void SetLookat();

	GLuint m_indicatorTexture;

	Vehicle *m_insideVehicle;
	float *m_lastElevation;
	Sphere m_charsphere;
};