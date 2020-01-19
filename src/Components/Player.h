#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include "../Camera.h"
#include "CompRegistration.h"
#include "../Physics/CollisionSystem/collision_narrow.h"

class Player : public wind::Box
{
	public:
		Player(wind::real mRatio);
		~Player();
		void setState(const wind::Vector3& pos, const wind::Vector3& half);
		void loadMesh(const std::string& filePath);

		void gravityOff();

		void canSleep(const bool state);
		void setSleep(const bool state);

		void move(wind::Vector3 vel);
		void rotate(const wind::Quaternion &rot);

		void update(wind::real duration);

		void changePosition(wind::Vector3 pos);
		void draw();
		Mesh* getMesh();

		wind::RigidBody* getBody();
		Camera getCamera();
	private:
        bool once;
        wind::real bspPlaneOffset;
		wind::Vector3 offsetVector;

		Mesh* cube;
		Camera cam;
};

#endif
