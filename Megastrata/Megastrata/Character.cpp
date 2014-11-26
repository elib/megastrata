#include "Character.h"
#include "Camera.h"
#include "World.h"



#include "Vehicle.h"


#define TRAVEL_SPEED (1.0f/1100.0f)

Character::Character(void)
{
	m_insideVehicle = NULL;
	m_lastElevation = NULL;

	Reset();

	m_charsphere.origin = Position;
	m_charsphere.radius = CHARACTER_HEIGHT;

	m_indicatorTexture = -1;
}


void Character::Go(int forward, int right, int up, bool snap, int ms)
{
	//forward and right are either 1, -1, or 0.

	Vector3d delta =	Vector3d( cos(m_turnangle) * forward,
								sin(m_turnangle) * forward,
								0.0f);

	//add strafe
	delta = delta +		Vector3d( cos(m_turnangle - HALF_PI) * right,
								sin(m_turnangle - HALF_PI) * right,
								0.0f);

	if(!snap)
	{
		delta = delta + Vector3d(0,0,up);
	}

	delta = delta.Normalize(); //get normalized vector
	delta = delta * ms * TRAVEL_SPEED; //multiply by time and speed

	if(delta.Magnitude() > EPSILON)
		World::theWorld->ConstrainToIndoors(Position, delta);

	if(snap)
	{
		float currfloor = PHYSICAL_HEIGHT_FACTOR * round(Position.z / PHYSICAL_HEIGHT_FACTOR);
		Position.z = currfloor + CHARACTER_HEIGHT;
	}

	if((delta.Magnitude() > EPSILON) || snap)
	{
		//trigger event
		NotifyChangedPosition();
		SetLookat();
	}
}

void Character::SetLookat()
{
	//find location on spherical envelope
	Lookat.x = cos(m_turnangle) * sin(m_upangle)	+ Position.x;
	Lookat.y = sin(m_turnangle) * sin(m_upangle)	+ Position.y;
	Lookat.z = cos(m_upangle)						+ Position.z;
}

string Character::GetLookDirection()
{
	string thedir("");
	float theangle = (m_turnangle - FOURTH_PI); // adjust so 0 is a bit left
	theangle = -(theangle - (TWICE_PI * floor(theangle / TWICE_PI))); //find fraction part
	if(theangle < 0) theangle += TWICE_PI; //ensure positive

	//find quadrant
	if(theangle < HALF_PI)
		thedir = "East";
	else if(theangle < PI)
		thedir = "South";
	else if(theangle < PI + HALF_PI)
		thedir = "West";
	else
		thedir = "North";

	return thedir;
}

void Character::ChangeLook(float dx, float dy)
{
	m_turnangle += dx; //wraps 360 degrees

	m_upangle -= dy; //artificially stop at convenient spots
	m_upangle = MAX(FOURTH_PI, m_upangle);
	m_upangle = MIN(3 * FOURTH_PI, m_upangle);

	SetLookat();
}

void Character::Reset()
{
	Camera::Reset();

	LeaveElevator();

	Position.x = 0.0f;
	Position.y = 0.0f;
	Position.z = CHARACTER_HEIGHT;

	m_turnangle = 0.0f;
	m_upangle = HALF_PI;

	SetLookat();
}

void Character::InitializeTexture()
{
	if(m_indicatorTexture != -1)
		return;

	//do texture for indicator
	glGenTextures(1, &m_indicatorTexture);

	GLubyte color[] = {128, 255, 255};

	glBindTexture(GL_TEXTURE_2D, m_indicatorTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Mag Filter

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, color);
}

void Character::RenderDirectionIndicator(float height)
{
	InitializeTexture();

	{
		AutoMatrixStack stack;

		glTranslatef(Position.x, Position.y, height);
		glScalef(1.0, 1.0f, 0.2f);
		glRotatef(-90, 1.0, 0, 0);
		glRotatef(DEG(-m_turnangle) + 90.0f, 0, 1.0f, 0);
		glTranslatef(0,0,-GRID_SIZE/6);
		glBindTexture(GL_TEXTURE_2D, m_indicatorTexture);
		glutSolidCone(GRID_SIZE/8, GRID_SIZE/3, 20, 5);
	}
}

void Character::OnVehicleArriveStation(Vehicle *vehicle)
{
	if(m_insideVehicle == vehicle)
	{
		LeaveElevator();

		Position.z = floor(Position.z) + CHARACTER_HEIGHT;
		//trigger event
		NotifyChangedPosition();
	}
}

void Character::OnVehicleDepartStation(Vehicle *vehicle)
{
	//check if we are inside
	if(vehicle->InVehicle(m_charsphere))
	{
		LOG("Vehicle 0x%x (belongs to 0x%x) is carrying character UP!", vehicle, vehicle->collection_id);
		vehicle->GetPosition().Log("Vehicle");
		m_insideVehicle = vehicle;
	}
}

void Character::OnVehicleMove(Vehicle *vehicle)
{
	if(m_insideVehicle != vehicle)
	{
		//check if we have leapt up/down a level
		//only jump to it if we're in transit at the time.
		if(!vehicle->InStation() && vehicle->InVehicle(m_charsphere))
		{
			m_insideVehicle = vehicle;
			
			LOG("Vehicle 0x%x (belongs to 0x%x) has captured the character!", vehicle, vehicle->collection_id);
			vehicle->GetPosition().Log("Vehicle");
		}
	}

	if(m_insideVehicle == vehicle)
	{
		float thisZ = vehicle->GetPosition().z;

		if(m_lastElevation != NULL)
		{
			//get difference (I don't think epsilon is needed here, the difference should be significant)
			if(thisZ < (*m_lastElevation))
			{
				thisZ += PHYSICAL_HEIGHT_FACTOR;
			}
		}
		else
		{
			m_lastElevation = new float();
		}

		Position.z = thisZ + CHARACTER_HEIGHT;
		//trigger event
		NotifyChangedPosition();

		(*m_lastElevation) = thisZ;

		SetLookat();
	}
}

void Character::NotifyChangedPosition()
{
	m_charsphere.origin = Position;
	World::theWorld->m_CharacterEventRegistry.CharacterMove(Position);
}

void Character::LeaveElevator()
{
	m_insideVehicle = NULL;

	if(m_lastElevation != NULL)
		delete m_lastElevation;

	m_lastElevation = NULL;
}