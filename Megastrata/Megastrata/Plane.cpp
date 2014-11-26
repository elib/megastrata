#include "Plane.h"

Plane::Plane()
{
	D = 0;
}

void Plane::Initialize(Vector3d theOrigin, Vector3d v1, Vector3d v2, Vector3d v3, Vector3d v4)
{
	origin = theOrigin;
	normal = ((v2 - v1).Cross((v3 - v1))).Normalize();

	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
	vertices[3] = v4;

	//see definion of D: http://mathworld.wolfram.com/Plane.html
	//[noob reference]
	D = -(v1.Dot(normal));

	m_BoundingSphere.SetBoundingSphere(v1, v2, v3, v4);
}

bool Plane::PointInQuad(Vector3d point)
{
	Vector3d u_vector = vertices[1] - vertices[0];
	Vector3d v_vector = vertices[3] - vertices[0];

	float max_u = (vertices[1] - vertices[0]).Dot(u_vector);
	float max_v = (vertices[3] - vertices[0]).Dot(v_vector);

	float val_u = (point - vertices[0]).Dot(u_vector);
	float val_v = (point - vertices[0]).Dot(v_vector);

	return (val_u >= 0 && val_u <= max_u && val_v >= 0 && val_v <= max_v);

}

Vector3d Plane::ClosestPointOnQuad(Vector3d p)
{
	Vector3d closest[4];
	for(int i = 0; i < 4; i++)
	{
		closest[i] = ClosestPointOnLine(vertices[i], vertices[(i+1)%4], p);
	}

	float dis = (closest[0] - p).MagnitudeSquared();
	int index = 0;
	for(int i = 1; i < 4; i++)
	{
		float tmp;
		tmp = (closest[i] - p).MagnitudeSquared();
		if(tmp < dis)
		{
			dis = tmp;
			index = i;
		}
	}

	return closest[index];
}


Vector3d Plane::ClosestPointOnLine(Vector3d a, Vector3d b, Vector3d p)
{
   // Determine t (the length of the vector from a to p)

   Vector3d c = p - a;
   Vector3d V = (b - a).Normalize(); //Normalized vector [b – a];
   float d = (a - b).Magnitude(); //double d = distance from a to b;
   float t = V.Dot(c);

   // Check to see if t is beyond the extents of the line segment
   if (t < 0) return a;
   if (t > d) return b;
 
   // Return the point between a and b
   V = V.SetLength(t); //set length of V to t;
   return a + V;
}

bool Plane::IsRelevant(Sphere testSphere)
{
	return testSphere.TestIntersection(m_BoundingSphere);
}