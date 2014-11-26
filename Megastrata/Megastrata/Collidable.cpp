#include "Collidable.h"

#include "World.h"

void CollidablePlane::Initialize(Vector3d v1, Vector3d v2, Vector3d v3, Vector3d v4)
{
	m_boundingSphere.SetBoundingSphere(v1, v2, v3, v4);
	m_plane.Initialize(m_boundingSphere.origin, v1, v2, v3, v4);
}