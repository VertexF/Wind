#ifndef PLANE_H
#define PLANE_H

#include "CompRegistration.h"
#include "../Physics/CollisionSystem/collision_narrow.h"
#include "../Graphics/ShaderProgram3D.h"

class Wall : public wind::Plane
{
	public:
		Wall();
		~Wall();
		void setState(const wind::Vector3& pos, wind::Vector3 dir);
		void initRotation(const wind::Vector3& axis, const wind::Vector3& pos, const wind::real& angle);
		void loadMesh(const std::string& filePath);
		void update(wind::real duration);
		void draw();
		wind::RigidBody* getBody();
		Mesh* getMesh();
	private:
		Mesh* mesh;
		//Only in this primitive that needs a body build here this is so we can render it.
		//This has nothing to do with the physics.
		//wind::RigidBody* body;
};

#endif
