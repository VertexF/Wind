#include "Camera.h"

Camera::Camera(const wind::Vector3& pos, wind::real fov, wind::real aspect, wind::real zNear, wind::real zFar)
	: rotation(0.0, 0.0, 0.0, 0.0),
	fov(fov),
	aspect(aspect),
	zNear(zNear),
	zFar(zFar)
{
	forward = wind::Vector3(0.0, 0.0, 1.0);
	up = wind::Vector3(0.0, 1.0, 0.0);

	forward.normalise();
	up.normalise();

	body.setPosition(pos);
	body.setOrientation(0.0, 0.0, 0.0, 0.0);
	body.setVelocity(0.0, 0.0, 0.0);
	body.setRotation(0.0, 0.0, 0.0);
	halfSize = wind::Vector3(1.0, 1.0, 1.0);

	wind::real mass = halfSize.x * halfSize.z * halfSize.y * 0.8;
	body.setInverseMass(mass);

	wind::Matrix3 tensor;
    tensor.setBlockInertiaTensor(halfSize, mass);
    body.setInertiaTensor(tensor);

	body.setLinearDamping(0.95);
	body.setAngularDamping(0.8);
	body.clearAccumulator();

	body.setCanSleep(false);
	body.setAwake();

	body.calculateDerivedData();
}

Camera::~Camera()
{
}

void Camera::resetPosition(wind::Vector3 pos)
{
    body.setPosition(pos);
	body.setOrientation(0.0, 0.0, 0.0, 0.0);
	body.setVelocity(0.0, 0.0, 0.0);
	body.setRotation(0.0, 0.0, 0.0);
}

void Camera::move(wind::Vector3 vel)
{
	body.setVelocity(vel);
}

wind::Vector3 Camera::getLeft()
{
	wind::Vector3 left = forward % up;
	left.normalise();
	return left;
}

wind::Vector3 Camera::getRight()
{
	wind::Vector3 right = up % forward;
	right.normalise();
	return right;
}

void Camera::rotate(const wind::Quaternion &rot)
{
	rotation = rot;
	body.setRotation(wind::Vector3().rotate(rot));
}

GLfloat* Camera::getCameraInWorldSpace()
{
    wind::Matrix4 trans = body.getTransform();
    trans.fillGLArray(cameraInWorld);
    return cameraInWorld;
}

void Camera::updateCamera(wind::real duration)
{
	body.integrate(duration);
}

/**
	These functions return the camera translation and rotation matrices.
*/
wind::Matrix4x4 Camera::getCameraRotation()
{
	wind::Matrix4x4 cameraRotation = cameraRotation.initRotation(rotation.getForward(), rotation.getUp(), rotation.getRight());
	return cameraRotation;
}

wind::Matrix4x4 Camera::getCameraTranslation()
{
	wind::Matrix4x4 cameraTranslation;
    cameraTranslation.setTranslation(-body.getPosition().x, -body.getPosition().y, -body.getPosition().z);

	return cameraTranslation;
}

wind::Matrix4x4 Camera::getLookAt()
{
	return view.lookAt(wind::Vector3(0.0 ,0.0, 0.0), rotation.getForward(), rotation.getUp());
}

/**
	This takes the right hand persective calculations and the right hand look at 4 by 4 matrices and muiplies them together and gets the perspective.
*/
wind::Matrix4x4 Camera::getViewProjection()
{
	return projection.perspectiveRH(fov, aspect, zNear, zFar);
}

wind::Matrix4x4 Camera::getVP()
{
	return getViewProjection() * getLookAt() * getCameraTranslation();
}
