#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include "Physics/include/Core.h"
#include "Physics/include/Body.h"

namespace 
{
    const wind::Vector3 yAxis(0.0, 1.0, 0.0);
    const wind::Vector3 xAxis(1.0, 0.0, 0.0);
};

namespace wind
{
class Camera
{
public:
    Camera(const Vector3& pos, real fovy, real aspect, real zNear, real zFar);
    ~Camera();

    void resetPosition(const Vector3& pos);

    Vector3 getForward() { return _forward; }
    Vector3 getUp() { return _up; }

    //These function returns a left and right vectors
    Vector3 getLeft();
    Vector3 getRight();

    //This function uses quaternion to rotate on a certain axis using angles in degrees.
    void rotate(const Quaternion &rot);

    void move(const Vector3 &vel);

    //This function takes the camera body aka camera space and returns a point in world space in a GLfloat matrix.
    GLfloat* getCameraInWorldSpace();

    void updateCamera(real duration);

    RigidBody getBody() { return _body; }

    Matrix4x4 getCameraRotation();
    Matrix4x4 getCameraTranslation();

    Matrix4x4 getLookAt();
    Matrix4x4 getViewProjection();
    Matrix4x4 getVP();

private:
    Matrix4x4 _projection;
    Matrix4x4 _view;
    Quaternion _rotation;

    Vector3 _forward;
    Vector3 _up;
    Vector3 _halfSize;
    RigidBody _body;

    GLfloat* _cameraInWorld;
    real _fov;
    real _aspect;
    real _zNear;
    real _zFar;
};
}; //wind
#endif
