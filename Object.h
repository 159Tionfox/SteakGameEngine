#ifndef OBJECT_H
#define OBJECT_H

#include "Library.h"

class Object
{
public:
	Object();
	~Object();

	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float row);

	void Translate(float x, float y, float z, float pitch, float yaw, float row);
	void Position(float x, float y, float z);
	void Rotate(float pitch, float yaw, float row);

public:
	//Now
	QVector3D up;
	QVector3D postion;
	QVector3D rotation;
	QVector3D forward;
};

#endif // OBJECT_H