#pragma once

#include <utils.h>
#include "Vector3d.h"

class Sphere
{
public:
	
	Sphere();

	bool TestIntersection(Sphere other);
	void SetBoundingSphere(Vector3d v1, Vector3d v2, Vector3d v3, Vector3d v4);

	Vector3d origin;
	float radius;
};