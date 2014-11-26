#pragma once
#include "WindowMapping.h"

//Abstract class representing a drop-in viewport.
class ViewPort
{
public:
	virtual void SetLightingAndCamera() = 0;
	void SetViewportSize(int w, int h);

	virtual void DoFrame(int msElapsed, ElapsedTime absolutetime, int *keysDown) { }
	virtual void Update(bool force = false) = 0;
	virtual void Render() = 0;

	virtual void PanMouse(float dx, float dy) = 0;
	virtual void ZoomMouse(float dx, float dy) = 0;

	void GetWorldCoordinates(float &x, float &y);

	ViewPort(void);
	virtual ~ViewPort(void);

	int m_width;
	int m_height;

protected:
	WindowMapping m_mapping2d;
};