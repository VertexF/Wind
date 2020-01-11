#ifndef PWORLD_H
#define PWORLD_H
#include <vector>
#include "pfgen.h"
#include "plinks.h"

namespace wind
{
    /**
        This class will hold the demo.
    */
    class ParticleWorld
    {
        public:
            typedef std::vector<Particle*> Particles;
            //This vector will hold all the particle contacts register.
            typedef std::vector<ParticleContactGenerator*> ContactGenerator;

            /**
                This will create a new particle simulator given the number of contacts per frame.
                You can optionally give a number of contact resolution.
                We will us twice the number contacts will be used.
            */
            ParticleWorld(unsigned maxContacts, unsigned iteration = 0);
            ~ParticleWorld();
        protected:
            //This will hold the particles.
            Particles mParticle;
        public:
            /**
                This function will set up the engine to run each engine frame.
                First it will clear all forces accumulators each frame.
                After this function call we will add all the forces back to themselves.
            */
            void startFrame();

            //Calls each of the registered contact generators their contacts. Returns the number of contacts generated.
            unsigned generateContacts();

            //This function integrate all the particles forward given by a duration.
            void integrate(real Duration);

            //This function of course runs all the physics calculations.
            void runPhysics(real Duration);

            //Returns a list of particles
            Particles& getParticles();

            //Returns a list of contacts
            ContactGenerator& getContacts();

            //Returns a list of force registry.
            ParticleForceRegistry& getRegistry();

        protected:

            //This is the particle force register for all the particles in the demo.
            ParticleForceRegistry mRegistry;

            //This unsigned int hold the max number of contacts that are allowed per frame.
            unsigned mMaxContact;

            //The particle contact generator for resolving collisions with impulse.
            ParticleContactResolver mResolver;

            //This is the contactGenerator for all the particles.
            ContactGenerator mConGenerator;

            //This will hold a list of contacts.
            ParticleContact* mContact;

            //This is true if the world should calculate the number of iterations to give the contact resolver at each frame.
            bool mCalculateIterations;
    };

    //This takes the defined type of particles and contacts them with the ground.
    class GroundContacts : public ParticleContactGenerator
    {
        public:
            void Init(ParticleWorld::Particles* particles);
            //This function handles particle contacts with the floor.
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
        private:
            ParticleWorld::Particles* particles;
    };
};

#endif // PWORLD_H
