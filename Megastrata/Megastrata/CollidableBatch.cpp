#include "CollidableBatch.h"
#include "World.h"

bool CollidableBatch::Collide(Vector3d &sourcePoint, Vector3d &velocityVector)
{
	Sphere playerSphere;
	playerSphere.origin = sourcePoint;
	playerSphere.radius = CHARACTER_HEIGHT;

	if(m_collidableCount == 0)
		LOG("No collidable items in this 9-grid. Not cool?");

	//starting destination
	Vector3d destinationPoint = sourcePoint + velocityVector;
	Vector3d previousDestination;
	Vector3d currentVelocity;

	int maxLoops = 5;
	int currentLoop = 0;

	//enter "recursive" loop - end when we go too deep, or if the updated destination doesn't change much
	while(currentLoop < maxLoops && (currentLoop == 0 || (previousDestination - destinationPoint).Magnitude() > EPSILON ))
	{
		currentVelocity = destinationPoint - sourcePoint;
		bool first = true;
		float distance;
		Vector3d slidingPlaneNormal;
		Vector3d untilCollision;

		//update destination sphere
		Sphere destinationSphere;
		destinationSphere.origin = destinationPoint;
		destinationSphere.radius = playerSphere.radius;

		for(int i = 0; i < m_collidableCount; i++)
		{
			if(m_collidablePlanes[i]->m_plane.IsRelevant(playerSphere)
				|| m_collidablePlanes[i]->m_plane.IsRelevant(destinationSphere))
			{
				//find first collision along velocity vector
				Vector3d revnormal = -m_collidablePlanes[i]->m_plane.normal;
				//set to magnitude of radius
				revnormal = revnormal * playerSphere.radius;
				//add to origin of sphere, this is the point on the sphere that should come in contact with the plane.
				Vector3d contactPoint = revnormal + playerSphere.origin;

				//pretend plane is infinite.
				//if delta is the change in position, use that as the direction of the ray.
				Vector3d dir = currentVelocity.Normalize();
				//t = -(normal . contactPoint + D) / normal . direction
				float Vd = (m_collidablePlanes[i]->m_plane.normal.Dot(dir));

				//reject if parallel to plane or from wrong side of plane
				if(!((ABS(Vd) < EPSILON) || (Vd > 0)))
				{
					float t = - ((m_collidablePlanes[i]->m_plane.normal.Dot(contactPoint) + m_collidablePlanes[i]->m_plane.D) / Vd);

					//t represents the distance to the collision from the contact point on the sphere.
					//if it's negative, it means we already passed the destination.
					//if it's positive, it means we're on a collision course.

					//we subtract currentVelocity.Magnitude() from t, and see if it's negative - it means we will hit the wall during this movement.
					if(ABS(t) < currentVelocity.Magnitude())
					{
						//1: get as close as possible
						Vector3d tempUntilCollision = dir * t;
						//LOG("untilCollision: %f", untilCollision.Magnitude());
						Vector3d closestPossible = contactPoint + tempUntilCollision;

						//2: discover what part of the quad we are touching
						if(m_collidablePlanes[i]->m_plane.PointInQuad(closestPossible))
						{
							if(first || tempUntilCollision.Magnitude() < distance)
							{
								slidingPlaneNormal = m_collidablePlanes[i]->m_plane.normal;
								untilCollision = tempUntilCollision;
								distance = tempUntilCollision.Magnitude();
								first = false;
							}
						}
						else
						{
							//we are not going to collide with a quad's face.
							//we need to do some heavy lifting now. More checks, this time against vertices and edges.

							//find the closes point on the quad's perimeter to the plane collision point.
							Vector3d polygonCollisionPoint = m_collidablePlanes[i]->m_plane.ClosestPointOnQuad(contactPoint + dir * t);
							//reverse normal is the negative normalized direction vector (current velocity)
							Vector3d revVelNorm = -dir;

							//reverse intersect from the polygon to the sphere, along -velocity
							Vector3d dst = polygonCollisionPoint - sourcePoint;
							float B = dst.Dot(revVelNorm);
							float C = dst.Dot(dst) - playerSphere.radius * playerSphere.radius;
							float D = B*B - C;
							if(D >= 0.0)
							{
								float dis = -B - sqrtf(D);

								if(dis - currentVelocity.Magnitude() < EPSILON)
								{
									if(first || dis < distance)
									{
										//dir is unit vector in direction of velocity
										//dis is the distance until collision (as discovered above)
										untilCollision = dir * dis;

										Vector3d sphereCollisionPoint = polygonCollisionPoint - untilCollision;
										slidingPlaneNormal = (sourcePoint - sphereCollisionPoint).Normalize();

										distance = dis;
										first = false;
									}
								}
							}
						} 
						//else, we don't even touch the quad! Leave Britney alone!
					} //end if (distance to plane smaller than desired velocity
				} //end if (direction of velocity is relevant to plane)
			} //end if (in bounding sphere of plane)
		} //end iteration (on all collidables)

		//done with this iteration
		if(first)
		{
			//no collisions.
			previousDestination = destinationPoint; //bail
		}
		else
		{
			//subtract the movement necessary to make contact
			Vector3d overstep = currentVelocity - untilCollision;
			
			//project overstep onto the sliding plane
			Vector3d projection = overstep - slidingPlaneNormal * slidingPlaneNormal.Dot(overstep);

			//prepare for next iteration.
			previousDestination = destinationPoint;

			//update new destination
			destinationPoint = sourcePoint + projection + untilCollision;
		}
		currentLoop++; //keep us within a plausable boundary of recursions
	} //end "recursion" loop


	//compare magnitude of velocities
	float currmag = currentVelocity.Magnitude();
	float mag = velocityVector.Magnitude();
	if(currmag > mag*2)
		LOG("omg, we're going farther than ever! old: %f, new: %f", mag, currmag);

	if((currentVelocity.Magnitude() > EPSILON) && (velocityVector.Magnitude() > EPSILON))
	{
		float theangle = currentVelocity.AngleTo(velocityVector);
		if(ABS(theangle) > HALF_PI)
			LOG("Woah we are going somewhere else! %f degrees!", DEG(theangle));
	}

	if(currentLoop == 5)
		LOG("hey wow, we've been colliding a lot");

	sourcePoint = destinationPoint;
	return true;
}

void CollidableBatch::AddCollisionPlane(CollidablePlane *plane)
{
	if(m_collidableCount >= MAX_BATCH_COLLIDABLES)
		return;

	m_collidablePlanes[m_collidableCount++] = plane;
}

void CollidableBatch::ClearCollidables()
{
	m_collidableCount = 0;
}