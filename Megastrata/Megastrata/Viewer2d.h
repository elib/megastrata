#pragma once

#include <utils.h>
#include <string>
#include "WindowMapping.h"

#include "Vector3d.h"
#include "ViewPort.h"
#include "LayerManager.h"

class Viewer2d: public ViewPort
{
public:
	//please call this first.
	virtual void SetViewportSize(int w, int h);

	virtual void SetLightingAndCamera();

	void Translate(float delta_x, float delta_y);
	void Scale(float scale_x, float scale_y);

	virtual void PanMouse(float dx, float dy);
	virtual void ZoomMouse(float dx, float dy);

	void GetWorldCoordinates(float &x, float &y);

	void ChangeParameter(float newParameter);
	void IncrementParameter(int variable, float delta);

	virtual void Update(bool force = false);
	virtual void Render();

	Viewer2d(void);

private:
	void UpdateWindowMapper();
	LayerManager *m_LayerManager;
};