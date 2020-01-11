#ifndef CONTACT_H
#define CONTACT_H

#include <vector>
#include <memory>
#include <iterator>
#include <iostream>

#include "../include/precision.h"
#include "../include/Core.h"
#include "../include/Body.h"

namespace wind
{
    /**
        This class supports the contacts and returns the relevant contact data to be used.
    */

    class Contact
    {
        friend class ContactResolver;

        public:
            //The 2 rigid bodies that the contacts will be happening on
            RigidBody* body[2];

            //This float supports the friction of the rigid bodies.
            real friction;

            //This float stored data of the restitution of the contact.
            real restitution;

            //This vector3 stores the contact point of the object.
            Vector3 contactPoint;

            //This vector3 stores the contact normal of the contact.
            //In this case this is purely to know the direction of the contact.
            Vector3 contactNormal;

            //This float stores the penetration depth of the contact.
            real penetration;

            //This function sets up data that doesn't need a contact set up.
            void setContactData(RigidBody* bodyOne, RigidBody* bodyTwo, real friction, real restitution);

            //Note: Currently this function is empty needs addition support to support it.
            void matchAwakeState();

        protected:
            //This matrix 3 by 3 will hold the contact to world.
            Matrix3 contactToWorld;

            //This vector holds the closing velocity when a contact happens.
            Vector3 closingVelocity;

            //This vector holds the the desired change in velocity when there is a contact.
            real desiredChangedVelocity;

            //This vector holds the contact velocity in local space.
            Vector3 contactVelocity;

            // This vector holds the world space position of the contact point relative to centre of each body.
            //This helps to calculate the intervales of the collision detection.
            Vector3 relativeContactPosition[2];

            /**
                This function calculates internal data from state data.
                This function is ran before any of the resolutions begin.
                This function should never be called manually.
            */
            void calculateInternals(real duration);

            /**
                This function swaps the bodies and the contact normal around.
                This functionality is supported by calling the calculate Internals function.
            */
            void swapBodies();

            /**
                This function awakes rigid body if they have been contacted.
                NOTE: Currently does nothing will be implemented on a later date.
            */
            void makeAwake();

            /**
                This function calculates and sets the internal value for the desired delta velocity.
                Which supports the calculateInternals function.
            */
            void calculateDesiredDeltaVelocity(real duration);

            /**
                This function calculates the contact point on a given velocity.
            */
            Vector3 calculateLocalVelocity(unsigned int index, real duration);

            /**
                This function calculates the orthonormal basis for the contact point.
                This will use the X-axis as the contact normal for the contact orthonormal.
                This contact orthonormal will based on the primary friction direction for anisotropic friction or a random orientation for isotropic friction.
            */
            inline void calculateContactBasis();

			/**
				This function applies the impulse and returns the change in velocity.
			*/
			void applyImpulse(const Vector3 &impulse, RigidBody *body, Vector3 *velocityChange, Vector3 *rotationChange);

			void applyPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], real penetration);

			void applyVelocityChange(Vector3 velocityChange[2], Vector3 rotationChange[2]);

            /**
                Here we are calculating the fictionless impulse angular velocity and linear velocity for one or 2 rigid bodies
            */
            Vector3 calculateFrictionlessImpulse(Matrix3* inverseInertiaTensor);

            /**
                Here we are calculating the fictionl impulse angular velocity and linear velocity for one or 2 rigid bodies
            */
            Vector3 calculateFrictionImpulse(Matrix3* inverseInertiaTensor);

    };

    /**
        This class resolve the collision and will be used throughout the whole system.
    */
    class ContactResolver
    {
        public:
            //This holds the data of what current position iterations we have used.
            unsigned positionIterationsUsed;

            //This holds the velocity iterations when resolved at each iteration.
            unsigned velocityIterationsUsed;

            //Creates a new contact resolver object with the given number of iterations.
            ContactResolver(unsigned interations, real velocityEpsilon = static_cast<real>(0.01), real positionEpsilon = static_cast<real>(0.01));

            //Creates a new contact resolver object with the given number of iterations for position and velocity.
            ContactResolver(unsigned positionIterations, unsigned velocityIterations, real velocityEpsilon = static_cast<real>(0.01), real positionEpsilon = static_cast<real>(0.01));

			//This function sets up the epsilon values for the velocity and position. A good value to start with is 0.01
			void setEpsilon(const real &velocityEpsilon, const real &positionEpsilon);

			void setIteration(const real &velocityIteration, const real &positionIteration);

			//Checks to see if the values are valid for the calculations.
			bool isValid()
			{
				return((positionIterations > 0) && (velocityIterations > 0) &&
					(positionEpsilon >= 0.0) && (positionEpsilon >= 0.0));
			}

        public:
            //This holds the data of what current position iterations we have.
            unsigned positionIterations;

                        //This holds the velocity iterations when resolved.
            unsigned velocityIterations;

            //To avoid instability velocities we use this value to change very low numbers like 0.000001 to 0.01
            real velocityEpsilon;

            //To avoid instability velocities we use this value to change very low numbers like 0.000001 to 0.01
            real positionEpsilon;

            /**
                This function sets up the contacts to be processed. Which includes the awakening the body and configuring.
            */
            void prepareContacts(Contact* contactArray, unsigned int numContacts, real duration);

            /**
                This function resolves the collision by using the contact data as a parameters.
                So it will handle penetration and velocity of an object.
            */
            void resolveContact(Contact* contactArray, unsigned int numContacts, real duration);

			/**
                This function works through each collision and contact point and adjust the linear and angular velocity of the rigid body
            */
			void adjustVelocities(Contact* contactArray, unsigned int numContacts, real duration);

            /**
                This function works through each collision and contact point and adjust the linear and angular position of the rigid body
            */
            void adjustPositions(Contact* contactArray, unsigned int numContacts, real duration);

       private:

            //Keeps track of the internal setting is valid
            bool valid;
    };

    /**
        A virtual base class that starts the vtable for a contact generator.
    */
    class ContactGenerator
    {
        virtual unsigned addContact(Contact* contact, unsigned int limit) const = 0;
    };
};

#endif // CONTACT_H_INCLUDED
