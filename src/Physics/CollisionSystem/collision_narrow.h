#ifndef COLLISION_NARROW_H
#define COLLISION_NARROW_H

#include <memory>
#include <vector>
#include <stdio.h>
#include <iostream>

//All includes are already in contact.
#include "contact.h"

/**
    Each collision algorithm will need 2 rigid bodies to detect a collision between the two.
    What will be returned is 0 or a contact object.
    Coherence will be used to allow more than one object to be detected while touching.
*/

namespace wind
{
    class CollisionDetection;
    class IntersectionTests;
	class PlayerGeometry;

    /**
        This structure is used to contain information for the collision detector to use in building contact data.
    */
    struct CollisionData
    {
        //This contact stays and holds the first contact that has happened
        Contact* contactArray;

        //This is an array that we will be writing to for the collision data.
        Contact* contacts;

        //This is for the buffer which is the maximum collision an object can take.
        int contactsLeft;

        //Total number of contacts.
        unsigned contactCount;

        //Holds the friction information.
        real friction;

        //Holds the restitution data.
        real restitution;

        //Holds the tolerance for no-collided and collided objects.
        real tolerance;

        bool anyContactsLeft()
        {
            if(contactsLeft > 0)
            {
                return true;
            }

            return false;
        }

        //This function resets all the contacts.
        void reset(int maxContacts)
        {
            contacts = contactArray;
            contactsLeft = maxContacts;
            contactCount = 0;
        }

        //This function adds a new contact to the contact array.
        void addContact(unsigned int iteration)
        {
            contactsLeft -= iteration;
            contactCount += iteration;

            contacts += iteration;
        }
    };

    /**
        This class is the super class for all the geometry primitives
    */
    class Primitive
    {
        public:
            //This helps both the collision detection and the intersection class because the have access to the primitive class
            //Along with all the primitive polygons classes.
            friend class IntersectionTests;
            friend class CollisionDetection;
			friend class PlayerGeometry;

            RigidBody* body;
            //The offset the matrix will only handle rotation and orientation of the rigid body.
            //Matrix4 offset;

            //This function will let us calculate the geometry primitives.
            void calculateInternals();

            Vector3 getAxis(unsigned int index) const
            {
                return transform.getAxisVector(index);
            }

            const Matrix4& getTransform() const
            {
                return transform;
            }

        protected:
            Matrix4 transform;

    };

    /**
        This class only holds data on a sphere.
    */
    class Sphere : public Primitive
    {
        public:
            real radius;
    };

    /**
        This class only holds data on a plane.
        It's not a primitive because it shouldn't be moved.
    */
    class Plane : public Primitive
    {
        public:
            Vector3 direction;

            real offset;
    };

    /**
        This class can be treated as a axis aligned bounded box as well as a unligned bounded box.
    */
    class Box : public Primitive
    {
        public:
            //This holds the middle point.
            Vector3 halfSize;
    };

    class IntersectionTests
    {
        public:
            //This function does a quick test to see of the box has intersected the half space.
            static bool BoxAndHalfSpace(const Box& box, const Plane& plane);

            //This function tests to see if the boxes are intersecting.
            static bool BoxAndBox(const Box& first, const Box& second);

			//This function handles the intersection of a half space and a sphere.
			static bool SphereAndHalfSpace(const Sphere& sphere, const Plane& plane);

			//This function handles the intersection of a sphere and a sphere.
			static bool SphereAndSphere(const Sphere& first, const Sphere& second);
    };

    /**
        This class contains the collision detection algorithms
    */
    class CollisionDetection
    {
	public:
        //This functions handles the collision detection between a sphere and another sphere.
        static unsigned SphereAndSphere(const Sphere& first, const Sphere& second, CollisionData& data);

        //This function handles the collision detection algorithm between a plane and sphere.
        //These are common halfspace algorithms although it's not true plane, this type of collision is used for ceilings and wall.
        //In halfspace one side of the wall is completely solid so only one side of wall checks a collision.
        static unsigned SphereAndHalfSpace(const Sphere& sphere, const Plane& plane, CollisionData& data);

        //This function handles a collision for a true plane which means both side are checked.
        static unsigned SphereAndTruePlane(const Sphere& sphere, const Plane& plane, CollisionData& data);

        //This function handles a box and halfspace collision.
        static unsigned BoxAndHalfSpace(const Box& box, const Plane& plane, CollisionData& data);

        //This function handles a box and sphere collision.
        static unsigned BoxAndSphere(const Box& box, const Sphere& sphere, CollisionData& data);

        //This function detects collisions between a point and a box.
        static unsigned BoxAndPoint(const Box &box, const Vector3 &point, CollisionData& data);

        //This function handles box on box collision which uses the SAT to get all the contact information.
      unsigned BoxAndBox(const Box& first, const Box& second, CollisionData& data);
    };

	class PlayerGeometry
	{
		public:
			//This function handles box on box collision which uses the SAT to get all the contact information.
			unsigned BoxAndBox(const Box& first, const Box& second, CollisionData& data);
	};
};

#endif // COLLISION_NARROW_H_INCLUDED
