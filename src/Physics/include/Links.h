#ifndef LINKS_H
#define LINKS_H

#include "../CollisionSystem/contact.h"

/**
	This class is an extension to the rigid body class.
	It allows rigid bodys to remain suck together. 
*/
namespace wind
{
	class Joints : public ContactGenerator
	{
		public:
			/**
				Holds the 2 bodies that will be stuck together.
			*/
			RigidBody* body[2];

			/**
				Here we have a position vector that give the relative loction of the joints for each body
				This is done in world space.
			*/
			Vector3 position[2];

			/**
				This holds the maxium value of the movement of the joint before it is consider violated.
				Normal it is a epsilon value but you can increase the value this get the inelastic look.
			*/
			real error;

			/**
				This function handles the configuration of the joints.
			*/
			void set(RigidBody* a, const Vector3& a_pos, RigidBody* b, const Vector3& b_pos, real error);

			/**
				This is the function that generates the contact to restore if it violated.
			*/
			unsigned addContact(Contact* contact, unsigned limit) const;
	};
};

#endif