#ifndef CAMERA_H
#define CAMERA_H

#include <glew.h>
#include "Physics/include/Core.h"
#include "Physics/include/Body.h"

const static wind::Vector3 yAxis(0.0, 1.0, 0.0);
const static wind::Vector3 xAxis(1.0, 0.0, 0.0);

class Camera
{
	public:
		Camera(const wind::Vector3& pos, wind::real fovy, wind::real aspect, wind::real zNear, wind::real zFar);
		~Camera();

		void resetPosition(wind::Vector3 pos);

		wind::Vector3 getForward() { return forward; }
		wind::Vector3 getUp() { return up; }

		//These function returns a left and right vectors
		wind::Vector3 getLeft();
		wind::Vector3 getRight();

		//This function uses quaternion to rotate on a certain axis using angles in degrees.
		void rotate(const wind::Quaternion &rot);

		void move(wind::Vector3 vel);

        //This function takes the camera body aka camera space and returns a point in world space in a GLfloat matrix.
		GLfloat* getCameraInWorldSpace();

		void updateCamera(wind::real duration);

        wind::RigidBody getBody() { return body; }

		wind::Matrix4x4 getCameraRotation();
		wind::Matrix4x4 getCameraTranslation();

		wind::Matrix4x4 getLookAt();
		wind::Matrix4x4 getViewProjection();
		wind::Matrix4x4 getVP();

	private:
		wind::Matrix4x4 projection;
		wind::Matrix4x4 view;
		wind::Quaternion rotation;

		wind::Vector3 forward;
		wind::Vector3 up;
		wind::Vector3 halfSize;
		wind::RigidBody body;

		GLfloat *cameraInWorld;
		wind::real fov;
		wind::real aspect;
		wind::real zNear;
		wind::real zFar;
};

#endif
