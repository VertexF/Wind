#include "Links.h"

using namespace wind;

void Joints::set(RigidBody* a, const Vector3& a_pos, RigidBody* b, const Vector3& b_pos, real error)
{
	body[0] = a;
	body[1] = b;

	position[0] = a_pos;
	position[1] = b_pos;

	Joints::error = error;
}

unsigned Joints::addContact(Contact* contact, unsigned limit) const
{
	//Firstly we need to calculate the position of each connection point in world coordinates.
    Vector3 a_pos_world = body[0]->getPointInWorld(position[0]);
    Vector3 b_pos_world = body[1]->getPointInWorld(position[1]);

	//Next we calculate the lenght and normal of a joint.
	Vector3 ab_difference = b_pos_world - a_pos_world;
	Vector3 normal = ab_difference;
	normal.normalise();
	real lenght = ab_difference.magnitude();

	//We take the absolute lenght of joint and if it's longer than the error lenght we reset.
	if(std::abs(lenght) > error)
	{
		contact->body[0] = body[0];
		contact->body[1] = body[1];
		contact->contactNormal = normal;
		contact->contactPoint = (a_pos_world + b_pos_world) * 0.5;
		contact->penetration = lenght - error;
		contact->friction = 1.0;
		contact->restitution = 0;

		return 1;
	}
	return 0;
}
