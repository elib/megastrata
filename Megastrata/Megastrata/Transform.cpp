#include "Transform.h"

Transform::Transform(void)
{
	Translation.x = Translation.y = 0.0f;
	Scale.x = Scale.y = 1.0f;
}

void Transform::Apply(float &x, float &y)
{
	x /= Scale.x;
	y /= Scale.y;

	x += Translation.x;
	y += Translation.y;
}