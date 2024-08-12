#include "Camera.h"

Camera::Camera()
{
    up.setX(0);
    up.setY(1);
    up.setZ(0);

    cameraspeed = 1;
    camerapitch = 0.2;
    camerayaw = 0.3;
}

Camera::~Camera()
{
}

QMatrix4x4 Camera::GetViewMat()
{
    QMatrix4x4 mat;
    mat.lookAt(postion, postion + forward, up);
    return mat;
}
