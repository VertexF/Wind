#include "world.h"

using namespace wind;

void World::startFrame()
{
    for(RigidBodies::iterator bod = bodies.begin(); bod != bodies.end(); bod++)
	{
        (*bod)->calculateDerivedData();
		(*bod)->clearAccumulator();
    }
}

World::RigidBodies &World::getBodies()
{
    return bodies;
}

ForceRegistry &World::getRegistry()
{
    return registry;
}

void World::integrate(real duration)
{
    for(RigidBodies::iterator bod = bodies.begin(); bod != bodies.end(); bod++)
    {
        (*bod)->integrate(duration);
    }
}

void World::runPhysics(real duration, RigidBody* body)
{
    registry.updateForce(duration, body);

    integrate(duration);
}
