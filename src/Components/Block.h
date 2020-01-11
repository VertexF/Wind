#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <string>
#include "CompRegistration.h"
#include "../Physics/CollisionSystem/collision_narrow.h"
#include "../Graphics/ShaderProgram3D.h"

class Block : public wind::Box
{
	public:
		Block();
		~Block();
		void setState(const wind::Vector3& pos, const wind::Vector3& half);
		void loadMesh(const std::string& filePath);
		void gravityOff();
		void canSleep(const bool state);
		void setSleep(const bool state);
		void update(wind::real duration);
		void changePosition(wind::Vector3 pos);
		void draw();
		wind::RigidBody* getBody();
		Mesh* getMesh();
	private:
		Mesh* cube;
};

#endif
