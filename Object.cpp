#include "Object.h"

#define PI 3.1415926535

Object::Object()
{
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
}

Object::~Object()
{
}

void Object::SetPosition(float x, float y, float z)
{
	postion.setX(x);
	postion.setY(y);
	postion.setZ(z);
}

void Object::SetRotation(float pitch, float yaw, float row)
{
	rotation.setX(pitch);
	rotation.setY(yaw);
	rotation.setZ(row);

	//x = cos(x) * sin(y)	y = sin(x)	z = cos(x)*cos(y)
	forward.setX(cos(pitch * PI / 180) * sin(yaw * PI / 180));
	forward.setY(sin(pitch * PI / 180));
	forward.setZ(cos(pitch * PI / 180) * cos(yaw * PI / 180));
	forward = forward.normalized();
}

void Object::Translate(float x, float y, float z, float pitch, float yaw, float row)
{
	Position(x, y, z);
	Rotate(pitch, yaw, row);
}

void Object::Position(float x, float y, float z)
{
	postion += forward * z;
	postion += QVector3D::crossProduct(forward, up) * x;
	postion += up * y;
}

void Object::Rotate(float pitch, float yaw, float row)
{
	SetRotation(rotation.x() + pitch, rotation.y() + yaw, rotation.z() + row);
}
