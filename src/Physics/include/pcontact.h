#ifndef PCONTACT_H_INCLUDED
#define PCONTACT_H_INCLUDED
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include "particle.h"

/**
    This file will handle and control particle contact.
    It will use impulse based resolution with hard constraints.
    Objects may rebound.

    The contact has no function, it just holds the details of the contacts.

    Note to self: If you don't understand the maths read Maths of Hard Constraints.
*/

namespace wind
{
    class ParticleContactResolver;

    class ParticleContact
    {
        friend class ParticleContactResolver;

        public:
            //These are the two particles that are going to be used in contact, if we are contacting a solid object the second particle will be NULL.
            Particle* mParticles[2];

            //Holds the restitution coefficient normal of the contact.
            //This is public because for each type of surface that will collide will have a different coefficient of restitution.
            real mRestitution;

            //Holds the direction of the contact in world coordinates.
            Vector3 mContactNormal;

            //This data type holds the penetration depth of a collision.
            //Take not this is mainly handled in the collision detection part of a physics engine.
            real mPenetration;

            //This Vector will hold how much a object needs to move in a interpenitration resolution.
            Vector3 mParticleMovement[2];

        protected:
            //Resolves this contact for both velocity and interpenetration.
            void resolve(real Duration);

            //This function calculates the separating when contacted
            real calculateSepVelocity() const;
        private:
            //This function handles the impulse calculations of the collision.
            void resolveVelocity(real Duration);

            //This function handles the Interpenetration calculations of a collision.
            void resolveInterpenetration(real Duration);
    };

    class ParticleContactResolver
    {
        protected:
            //This will hold how iterations we are allowed to do.
            //Assigned in the constructor.
            unsigned mIteration;

            //The tracking iteration, keep track of how many times we have iterated over the resolver.
            unsigned mInterationUsed;
        public:
            //Creates a new contact resolver with the iteration limit per frame.
            ParticleContactResolver(unsigned iterations);

            //Function that allows to reset and stuff the total number of iteration we can do per frame.
            void SetIteration(unsigned iterations);

            //The big daddy resolver, resolves collision with different objects.
            /** Arguments:
                ParticleArray: will be an array of particle particle contact that need to be resolved, also why it is important to have a iteration limit, because so many particles.
                numContacts: will hold how many contacts we need to resolve.
                duration: This is the duration of the previous iteration step.
            */
            //void ResolveContact(std::vector<ParticleContact*> ContactArray, unsigned numContacts, real duration);

            void ResolveContact(ParticleContact *contactArray, unsigned numContacts, real duration);
    };

    /**This class is a virtual base class*/
    class ParticleContactGenerator
    {
        public:
            //This function means that the store the total of particles that being used in contacts.
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;
    };

};
#endif // PCONTACT_H_INCLUDED
