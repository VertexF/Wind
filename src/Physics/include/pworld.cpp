#include "pworld.h"

using namespace wind;

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iteration) : mMaxContact(maxContacts), mResolver(iteration)
{
    mContact = new ParticleContact[maxContacts];
    if(iteration == 0)
    {
        mCalculateIterations = true;
    }
    else
    {
        mCalculateIterations = false;
    }
}

ParticleWorld::~ParticleWorld()
{
    delete [] mContact;
}

void ParticleWorld::startFrame()
{
    for(Particles::iterator p = mParticle.begin(); p != mParticle.end(); p++)
    {
        //Clear everything every signal frame.
        (*p)->ClearAccumulator();
    }
}

unsigned ParticleWorld::generateContacts()
{
    unsigned limit = mMaxContact;
    ParticleContact* nextContact = mContact;

    for(ContactGenerator::iterator g = mConGenerator.begin(); g != mConGenerator.end(); g++)
    {
        unsigned used = (*g)->addContact(nextContact, limit);

        limit -= used;
        nextContact += used;

        //We have ran out of contacts :O that means we are miss contacts.
        if(limit <= 0)
        {
            break;
        }
    }

    //Return the number of contacts used.
    return mMaxContact - limit;
}

void ParticleWorld::integrate(real Duration)
{
    for(Particles::iterator p = mParticle.begin(); p != mParticle.end(); p++)
    {
        //Here we are removing all forces from the accumulator.
        (*p)->Intergrate(Duration);
    }
}

void ParticleWorld::runPhysics(real Duration)
{
    mRegistry.UpdateForce(Duration);

    integrate(Duration);

    unsigned usedContacts = generateContacts();

    if(usedContacts)
    {
        if(mCalculateIterations)
        {
            mResolver.SetIteration(usedContacts * 2);
        }

        mResolver.ResolveContact(mContact, usedContacts, Duration);
    }
}

ParticleWorld::Particles& ParticleWorld::getParticles()
{
    return mParticle;
}

ParticleWorld::ContactGenerator& ParticleWorld::getContacts()
{
    return mConGenerator;
}

ParticleForceRegistry& ParticleWorld::getRegistry()
{
    return mRegistry;
}

void GroundContacts::Init(ParticleWorld::Particles* particles)
{
    GroundContacts::particles = particles;
}

unsigned GroundContacts::addContact(ParticleContact* contact, unsigned limit) const
{
    unsigned Count = 0;
    for(ParticleWorld::Particles::iterator p = particles->begin(); p != particles->end(); p++)
    {
        real y = (*p)->GetPosition().y;

        //Here we have set up collisions
        if(y < 0.0f)
        {
            //This is how you program the impulse resolution for basic collisions.
            contact->mContactNormal = wind::Vector3::UP;
            contact->mParticles[0] = *p;
            contact->mParticles[1] = nullptr;
            contact->mPenetration = -y;
            contact->mRestitution = 0.2f;
            contact++;
            Count++;
        }

        if(Count >= limit)
        {
            std::cout << "Do we leave?" << std::endl;
            return Count;
        }
    }

    return Count;
}
