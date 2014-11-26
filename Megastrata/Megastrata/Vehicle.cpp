#include "Vehicle.h"
#include "World.h"
#include "UserPromptUtil.h"

Vehicle::Vehicle(void)
{
	height_delta = 0;
	is_in_station = false;
}

bool Vehicle::InVehicle(Sphere point)
{
	if(m_boundingSphere.TestIntersection(point))
	{
		return ABS(point.origin.x - position.x) < dimensions.x/2
			&& ABS(point.origin.y - position.y) < dimensions.y/2
			&& ABS(point.origin.z - position.z) < dimensions.z/2;
	}

	return false;
}

void Vehicle::Render()
{
	AutoMatrixStack stack;
	Vector3d corners[4];
	glTranslatef(position.x, position.y, position.z);

	if(visible)
	{
		{
			AutoMatrixStack stack;
		
			corners[0] = Vector3d(0, -0.5f,	0);
			corners[1] = Vector3d(0, -0.5f,	1.0);
			corners[2] = Vector3d(0, 0.5f,	1.0);
			corners[3] = Vector3d(0, 0.5f,	0);

			glRotatef(orientation, 0,0,1.0f);
			glScalef(dimensions.x, dimensions.y, dimensions.z);

			for(int q = -1; q < 2; q++)
			{
				AutoMatrixStack stack;
				glRotatef(90 * q, 0, 0, 1.0f);
				glTranslatef(-0.5, 0, 0);
				TessellatedQuad(corners, 1, 1);
			}
		}

		corners[0] = Vector3d(dimensions.x/2, dimensions.y/2,	0);
		corners[1] = Vector3d(dimensions.x/2, -dimensions.y/2,	0);
		corners[2] = Vector3d(-dimensions.x/2, -dimensions.y/2,	0);
		corners[3] = Vector3d(-dimensions.x/2, dimensions.y/2,	0);

		{
			AutoMatrixStack stack;
			glTranslatef(0, 0, dimensions.z);
			TessellatedQuad(corners, 1, 1);
			glTranslatef(0, 0, -dimensions.z);
			TessellatedQuad(corners, 1, 1);
		}
	}
}

float EaseInOut(float fractionaltime)
{
	float input = fractionaltime - 0.5f;
	return (input - (sinf(TWICE_PI * input) / TWICE_PI));
}


void Vehicle::Update(ElapsedTime absoluteTickCount)
{
	//simple script that goes up.
	
	//find start:
	int period = 10 * 1000;
	int time_into_cycle = absoluteTickCount % period;
	float fractional_time = (float)time_into_cycle / (float) period;
	float zdistance = EaseInOut(fractional_time);


	float total_height = PHYSICAL_HEIGHT_FACTOR;
	position.z = height_delta + total_height * (zdistance);
	
	visible = true;
	
	UpdateCollidables();

	//update parent entity if necessary:
	//arrive/depart station
	DetermineIfInStation(zdistance);
	//update of movement.
	NotifyChangedPosition();
}

void Vehicle::Initialize(Vector3d targetpos, Vector3d targetdimensions, float targetorientation)
{
	height_delta = targetpos.z;
	position = targetpos;
	orientation = targetorientation;
	dimensions = targetdimensions;
	
	m_boundingSphere.origin = position;
	m_boundingSphere.radius = MAX(MAX(dimensions.x, dimensions.y), dimensions.z);
	visible = false;
}

void Vehicle::GetCollidables(CollidableBatch *batch)
{
	m_collidables.AddCollidablesToBatch(batch);
}

void Vehicle::DetermineIfInStation(float zdistance)
{
	//if(!alive)
	//{
	//	UserPromptUtil::UserMessageBox("ARARARA!!!");
	//}

	//get current elevator height
	bool currentlyIn = (ABS(zdistance) < 0.001);

	if(currentlyIn != is_in_station)
	{
		if(currentlyIn)
		{
			//remove colliding surface ("door")
			//LOG("Arrived at station!");
			World::theWorld->m_VehicleEventRegistry.VehicleArriveStation(this);
		}
		else
		{
			//add colliding surface ("door")
			//LOG("Left station!");
			World::theWorld->m_VehicleEventRegistry.VehicleDepartStation(this);
		}
	}

	is_in_station = currentlyIn;
}

void Vehicle::UpdateCollidables()
{
	m_collidables.ClearCollidables();
	//place 3 standard walls

	Vector3d corners[4];
	corners[0] = Vector3d(0, -0.5f,	0);
	corners[1] = Vector3d(0, -0.5f,	1.0);
	corners[2] = Vector3d(0, 0.5f,	1.0);
	corners[3] = Vector3d(0, 0.5f,	0);

	//if(visible)
	{
		AutoMatrixStack stack;
		glTranslatef(position.x, position.y, position.z);
		
		glRotatef(orientation, 0,0,1.0f);
		glScalef(dimensions.x, dimensions.y, dimensions.z);

		for(int q = -1; q < 2; q++)
		{
			AutoMatrixStack stack;
			glRotatef(90 * q, 0, 0, 1.0f);
			glTranslatef(-0.5, 0, 0);
			glRotatef(180, 0,0,1.0f);

			//get current matrix
			float modelview[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
			m_collidables.AddCollisionPlane(modelview, corners[0], corners[1], corners[2], corners[3]);

			if(q == 0)
			{
				for(int b = 0; b < 4; b++)
					World::theWorld->AddDebugPoint(corners[b].Transform(modelview));
			}

		//	glutWireSphere(1.0f, 10, 10);
		}
	}

	if(!is_in_station)
	{
		//in transit. Two barriers: One in the traveling elevator, one on the ground.
		AutoMatrixStack stack;
		{
			AutoMatrixStack stack;
			glTranslatef(position.x, position.y, height_delta);
			glRotatef(orientation, 0,0,1.0f);
			glScalef(dimensions.x, dimensions.y, dimensions.z);
			glTranslatef(0.5, 0, 0);
			//rotate for ground level
			glRotatef(180, 0,0,1.0f);

			
			float modelview[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
			m_collidables.AddCollisionPlane(modelview, corners[0], corners[1], corners[2], corners[3]);
		}

		{
			AutoMatrixStack stack;
			glTranslatef(position.x, position.y, position.z);
			glRotatef(orientation, 0,0,1.0f);
			glScalef(dimensions.x, dimensions.y, dimensions.z);
			glTranslatef(0.5, 0, 0);

			//in transit
			float modelview[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
			m_collidables.AddCollisionPlane(modelview, corners[0], corners[1], corners[2], corners[3]);
			
			//for(int ggg = 0; ggg < 4; ggg++)
			//	World::theWorld->AddDebugPoint(corners[ggg].Transform(modelview));

//			World::theWorld->AddDebugPoint(((corners[0] + corners[1] + corners[2] + corners[3])/4).Transform(modelview));
		}
	}
}

const Vector3d Vehicle::GetPosition()
{
	return position;
}

void Vehicle::NotifyChangedPosition()
{
	m_boundingSphere.origin = position;
	World::theWorld->m_VehicleEventRegistry.VehicleMove(this);
}

bool Vehicle::InStation()
{
	return is_in_station;
}