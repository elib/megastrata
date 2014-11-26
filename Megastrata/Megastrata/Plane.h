#pragma once

#include <utils.h>
#include "Sphere.h"


class Plane
{
public:
	
	Plane();

	void Initialize(Vector3d theOrigin, Vector3d v1, Vector3d v2, Vector3d v3, Vector3d v4);
	Vector3d ClosestPointOnQuad(Vector3d p);

	bool IsRelevant(Sphere testSphere);

	bool PointInQuad(Vector3d point);

	Vector3d origin;
	Vector3d normal;
	Vector3d vertices[4];
	float D;
	Sphere m_BoundingSphere;
protected:
	Vector3d ClosestPointOnLine(Vector3d a, Vector3d b, Vector3d p);
	
};