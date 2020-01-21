#include "Camera.h"

namespace wind
{
/******************************************************************************/
Camera::Camera(const Vector3& pos, real fov, real aspect, real zNear, real zFar)
	: _rotation(0.0, 0.0, 0.0, 0.0), _fov(fov), _aspect(aspect), _zNear(zNear),
	_zFar(zFar), _forward(Vector3(0.0, 0.0, 1.0)), _up(Vector3(0.0, 1.0, 0.0)),
	_cameraInWorld(nullptr), _halfSize(1.0, 1.0, 1.0)
{
	_forward.normalise();
	_up.normalise();

	resetPosition(pos);

	real mass = _halfSize.x * _halfSize.z * _halfSize.y * 0.8;
	_body.setInverseMass(mass);

	Matrix3 tensor;
	tensor.setBlockInertiaTensor(_halfSize, mass);
	_body.setInertiaTensor(tensor);

	_body.setLinearDamping(0.95);
	_body.setAngularDamping(0.8);
	_body.clearAccumulator();

	_body.setCanSleep(false);
	_body.setAwake();

	_body.calculateDerivedData();
}

/******************************************************************************/
Camera::~Camera()
{
}

/******************************************************************************/
void Camera::resetPosition(const Vector3 &pos)
{
	_body.setPosition(pos);
	_body.setOrientation(0.0, 0.0, 0.0, 0.0);
	_body.setVelocity(0.0, 0.0, 0.0);
	_body.setRotation(0.0, 0.0, 0.0);
}

/******************************************************************************/
void Camera::move(const Vector3 &vel)
{
	_body.setVelocity(vel);
}

/******************************************************************************/
Vector3 Camera::getLeft()
{
	Vector3 left = _forward % _up;
	left.normalise();
	return left;
}

/******************************************************************************/
Vector3 Camera::getRight()
{
	wind::Vector3 right = _up % _forward;
	right.normalise();
	return right;
}

/******************************************************************************/
void Camera::rotate(const Quaternion& rot)
{
	_rotation = rot;
	_body.setRotation(Vector3().rotate(rot));
}

/******************************************************************************/
GLfloat* Camera::getCameraInWorldSpace() const
{
	_body.getTransform().fillGLArray(_cameraInWorld);
	return _cameraInWorld;
}

/******************************************************************************/
void Camera::updateCamera(real duration)
{
	_body.integrate(duration);
}

/******************************************************************************/
/**
	These functions return the camera translation and rotation matrices.
*/
Matrix4x4 Camera::getCameraRotation() const
{
	wind::Matrix4x4 cameraRotation;
	return cameraRotation.initRotation(_rotation.getForward(), _rotation.getUp(), _rotation.getRight());
}

/******************************************************************************/
Matrix4x4 Camera::getCameraTranslation() const
{
	wind::Matrix4x4 cameraTranslation;

	return cameraTranslation.setTranslation(-_body.getPosition().x, 
											-_body.getPosition().y, 
											-_body.getPosition().z);
}

/******************************************************************************/
Matrix4x4 Camera::getLookAt() const
{
	return _view.lookAt(Vector3(0.0, 0.0, 0.0), _rotation.getForward(), _rotation.getUp());
}

/******************************************************************************/
/**
	This takes the right hand persective calculations and the right hand look at
	4 by 4 matrices and muiplies them together and gets the perspective.
*/
Matrix4x4 Camera::getViewProjection() const
{
	return _projection.perspectiveRH(_fov, _aspect, _zNear, _zFar);
}

/******************************************************************************/
Matrix4x4 Camera::getVP() const
{
	Matrix4x4 viewProjection = getViewProjection() * getLookAt() * getCameraTranslation();
	return viewProjection;
}
}; //wind