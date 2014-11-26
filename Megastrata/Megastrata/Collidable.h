#pragma once
#include "Plane.h"


#define MAX_COLLIDABLE 40

class CollidablePlane
{
public:

	void Initialize(Vector3d v1, Vector3d v2, Vector3d v3, Vector3d v4);

	Sphere m_boundingSphere;

	Plane m_plane;

protected:
	
};