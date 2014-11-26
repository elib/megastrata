#include "Vector3d.h"

Vector3d::Vector3d(void)
{
	x = y = z = 0.0f;
}

Vector3d::Vector3d(float newX, float newY, float newZ)
{
	Initialize(newX, newY, newZ);
}

Vector3d::Vector3d(Vector3d const& source)
{
	Initialize(source.x, source.y, source.z);
}

void Vector3d::Initialize(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}


float Vector3d::Dot(Vector3d const& param)
{
	return (x * param.x + y * param.y + z * param.z);
}

Vector3d Vector3d::Cross(Vector3d const& param)
{
	Vector3d result;
	result.x = ((this->y * param.z) - (this->z * param.y)); //a2*b3 - a3*b2,
	result.y = ((this->z * param.x) - (this->x * param.z)); //a3*b1 - a1*b3
	result.z = ((this->x * param.y) - (this->y * param.x)); //a1*b2 - a2*b1
	return result;
}

Vector3d Vector3d::Transform(float *matrix)
{
	float coords[3];
	
	for(int i = 0; i < 3; i++)
	{
		//multiplying by (x,y,z,1)
		coords[i] = matrix[i] * x + matrix[i + 4] * y + matrix[i + 8] * z + matrix[i + 12];
	}

	return Vector3d(coords[0], coords[1], coords[2]);
}

const float Vector3d::MagnitudeSquared()
{
	//for quicker calculations when not needed.
	return x*x + y*y + z*z;
}

const float Vector3d::Magnitude()
{
	//pythagorean theorem FTW
	return(sqrt(MagnitudeSquared()));
}

Vector3d Vector3d::Normalize()
{
	float mag = Magnitude();
	if(mag == 0)
		return Vector3d(0,0,0);

	return Vector3d(x/mag, y/mag, z/mag);
}

Vector3d Vector3d::operator -(Vector3d const& source)
{
	return Vector3d(x - source.x, y - source.y, z - source.z);
}

Vector3d Vector3d::operator -()
{
	return Vector3d(-x, -y, -z);
}

Vector3d Vector3d::operator +(Vector3d const& source)
{
	return Vector3d(x + source.x, y + source.y, z + source.z);
}

Vector3d Vector3d::operator *(float amount)
{
	return Vector3d(x * amount, y * amount, z * amount);
}

Vector3d Vector3d::operator /(float amount)
{
	return Vector3d(x / amount, y / amount, z / amount);
}

Vector3d Vector3d::SetLength(float newlength)
{
	return (Normalize() * newlength);
}

void Vector3d::DebugRender()
{
	AutoMatrixStack stack;

	glTranslatef(x, y, z);
	glutWireSphere(0.1f, 10, 10);

}

void Vector3d::Log(char * prefix)
const {
#ifdef _DEBUG
	LOG("%s: vector output: (%f, %f, %f)", prefix, x, y, z);
#endif
}

float Vector3d::AngleTo(Vector3d target)
{
	float dot = Dot(target);
	dot = dot / (Magnitude() * target.Magnitude());
	return acosf(dot);
}


void Ray::DebugRender()
{
	Vector3d dir = direction.Normalize();
	float factor = 0.01f;

	float angle = acosf(dir.Dot(Vector3d(0,0,1)));
	Vector3d axis = dir.Cross(Vector3d(0,0,1));

	{
		AutoMatrixStack stack;
		glTranslatef(origin.x, origin.y, origin.z);
		glRotatef(-DEG(angle), axis.x, axis.y, axis.z);
		glutWireCone(factor, 3*factor, 10, 10);
	}
}