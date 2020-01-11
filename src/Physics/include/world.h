#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED
#include <vector>
#include "ForceGen.h"
#include "Body.h"

/**
    This file will support the simulation of each individual rigid body.
    Keeping track of the bodies and updates them
*/

namespace wind
{
    class World
    {
        public:
            typedef std::vector<RigidBody*> RigidBodies;

            ForceRegistry registry;

            RigidBodies bodies;

            //Restarts the forces for each frame
            void startFrame();

            //This function returns the force registry so we can integrate and add all the force to the same registry
            ForceRegistry& getRegistry();

            RigidBodies &getBodies();

            //Runs the integrator for each rigid body.
            void integrate(real duration);

            //This functions runs the physics for the rigid bodies and applies a force generator
            void runPhysics(real duration, RigidBody* body);
    };
};

#endif // WORLD_H_INCLUDED
