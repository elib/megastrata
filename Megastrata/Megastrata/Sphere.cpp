#include "Sphere.h"

Sphere::Sphere()
{
	radius = 0;
}

bool Sphere::TestIntersection(Sphere other)
{
	Vector3d centers = other.origin - origin;
	float distance = centers.Magnitude();
	
	return (distance < (other.radius + radius));
}

void Sphere::SetBoundingSphere(Vector3d v1, Vector3d v2, Vector3d v3, Vector3d v4)
{
	//origin is average location in 3d
	origin = (v1 + v2 + v3 + v4) / 4.0f;

	//radius of the sphere will be the longest distance to any of the corners.
	//find furthest vertex from the center:
	float radiusSquared = (v1 - origin).MagnitudeSquared();
	
	float radiusSquaredCandidate = (v2 - origin).MagnitudeSquared();
	if(radiusSquared < radiusSquaredCandidate) radiusSquared = radiusSquaredCandidate;
	
	radiusSquaredCandidate = (v3 - origin).MagnitudeSquared();
	if(radiusSquared < radiusSquaredCandidate) radiusSquared = radiusSquaredCandidate;
	
	radiusSquaredCandidate = (v4 - origin).MagnitudeSquared();
	if(radiusSquared < radiusSquaredCandidate) radiusSquared = radiusSquaredCandidate;

	radius = sqrt(radiusSquared);
}