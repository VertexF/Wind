#ifndef PLINKS_H_INCLUDED
#define PLINKS_H_INCLUDED

#include "particle.h"
#include "pcontact.h"

/**
    This class file will support particles that cannot be pulled apart.
    This works by simulating the contacts just like normal but reserving the contact normal so particles can be brought back together.
    Also the interpenetration depth of 2 objects is how far two particles have been pulled away from the cables limit.
*/

namespace wind
{
    class ParticleLink : public ParticleContactGenerator
    {
        public:
            //Of course we need the two particles to work with.
            Particle* mParticles[2];

            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;
        protected:

            //Getter function that gets the current length of the link.
            real currentLength() const;

    };

    /**
        This class handles a cable between two particles and generates a contact if the particles get too far apart.
    */
    class ParticleCable : public ParticleLink
    {
        public:
            //Holds the maximum length of the cable.
            real mMaxLength;

            //Holds the coefficient of restitution of the cable.
            real mRestitution;

            //Runs down the v-table to set up a contact generator so make the contacts work.
            //If there is no strain then it will return 0 and if there is strain then 1
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
    };

    class ParticleRod : public ParticleLink
    {
        public:
        //This will hold the length of the rod
        real mLength;

        //This will hold the algorithm for generating a rod.
        virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
    };

    /**This class adds constraints to particles like rods and stuff but they have a fixed anchor point.*/
    class ParticleConstraint : public ParticleContactGenerator
    {
        public:
            //This will hold particles together by these constraints.
            Particle* mParticle;

            //The anchor point to where these particles are stuck to.
            Vector3 mAnchor;
        protected:
            //This returns the length of the to the link.
            real currentLength() const;
        public:
            //We NEED to overwrite this add contact function for other classes so it is a pure virtual.
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;
    };

    /** This class supports cables with a anchor constraint point */
    class ParticleCableConstraint : public ParticleConstraint
    {
        public:
            //Max length of the cable.
            real mMaxLength;

            //You know what this :D
            real mRestitution;

        public:
            //This function fills out the constraint to make sure the cable is always full extended.
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
    };

    class ParticleRodConstraint : public ParticleConstraint
    {
        public:
            //Length of the rod.
            real mLength;

            //This function fills out the constraint to make sure the rod is never compressed or extended.
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
    };
};

#endif // PLINKS_H_INCLUDED
