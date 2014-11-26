#pragma once
#include <utils.h>
#include "Serializable.h"

class Vector3d: public Serializable
{
public:

	float x,y,z;

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);

	Vector3d(void);
	Vector3d(float newX, float newY, float newZ);
	Vector3d(Vector3d const& source);

	Vector3d operator -(Vector3d const& source);
	Vector3d operator -();
	Vector3d operator +(Vector3d const& source);
	Vector3d operator *(float amount);
	Vector3d operator /(float amount);

	float AngleTo(Vector3d target);

	Vector3d Cross(Vector3d const& param);
	float Dot(Vector3d const& param);

	Vector3d Transform(float *matrix);

	const float MagnitudeSquared();
	const float Magnitude();
	Vector3d Normalize();

	Vector3d SetLength(float newlength);

	void Initialize(float newX, float newY, float newZ);

	void Log(char* prefix) const;

	void DebugRender();
};

struct Ray
{
	Ray(Vector3d o, Vector3d d)
	{
		origin = o;
		direction = d;
	}

	Ray(){}

	void DebugRender();
	Vector3d origin, direction;
};