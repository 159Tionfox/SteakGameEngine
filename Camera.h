#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"
class Camera :
    public Object
{
public:
    Camera();
    ~Camera();

    QMatrix4x4 GetViewMat();

    float cameraspeed;
    float camerapitch;
    float camerayaw;
};

#endif // CAMERA_H
