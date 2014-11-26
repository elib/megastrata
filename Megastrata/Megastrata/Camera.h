#pragma once
#include <utils.h>
#include "Vector3d.h"
#include "Serializable.h"


class Camera : public Serializable
{
public:

	void CreateCamera();

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);

	

	virtual void Reset();

	Camera(void);
	virtual ~Camera(void);

	Vector3d Position;
	Vector3d Lookat;
};