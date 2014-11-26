#include "Camera.h"
#include "World.h"

Camera::Camera(void)
{
	Reset();
}

Camera::~Camera(void)
{
}


void Camera::CreateCamera()
{
	gluLookAt(	Position.x, Position.y, Position.z,
		Lookat.x, Lookat.y, Lookat.z,
		0.0, 0.0, 1.0);
}

void Camera::Reset()
{
	Position.x = 2.0f;
	Position.y = 5.0f;
	Position.z = 6.0f;

	Lookat.x = Lookat.y = Lookat.z = 0.0f;
}