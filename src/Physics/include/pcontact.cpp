#include "pcontact.h"

using namespace wind;

void ParticleContact::resolve(real Duration)
{
    resolveVelocity(Duration);
    resolveInterpenetration(Duration);
}

real ParticleContact::calculateSepVelocity() const
{
    Vector3 relativeVelocity = mParticles[0]->GetVelocity();
    if(mParticles[1])
    {
        relativeVelocity = relativeVelocity - mParticles[1]->GetVelocity();
    }

    return relativeVelocity * mContactNormal;
}

void ParticleContact::resolveVelocity(real Duration)
{
    //First thing we need to do is find the velocity in the direction of the contact.
    real separatingVelocity = calculateSepVelocity();

    //Then we it needs to be resolved at all.
    if(separatingVelocity > 0)
    {
        //The separating is either separating or stationary, so we just return.
        return;
    }

    //This is the new separating velocity we are going to work with.
    real newSepVelocity = -separatingVelocity * mRestitution;

    //Next we need to calculate and check the velocity build up due to acceleration.
    Vector3 accCausedVelocity = mParticles[0]->GetAcceleration();
    if(mParticles[1])
    {
        accCausedVelocity -= mParticles[1]->GetAcceleration();
    }

    real accCausedSepVelocity = accCausedVelocity * mContactNormal * Duration;

    //What this does her is it builds separating velocity due to acceleration build up we need to remove it.
    if(accCausedSepVelocity < 0)
    {
        newSepVelocity += mRestitution * accCausedSepVelocity;

        //This just checks we haven't removed more than there was.
        if(newSepVelocity < 0)
        {
            newSepVelocity = 0;
        }
    }

    real deltaVelocity = newSepVelocity - separatingVelocity;

    //Next we do something smart, in proportion to the particles inverse mass we change the velocity.
    //Those with a lower inverse mass have have higher mass so don't move as far or as fast.
    real totalInverseMass = mParticles[0]->GetInverseMass();
    if(mParticles[1])
    {
        totalInverseMass += mParticles[1]->GetInverseMass();
    }

    //We need to return because both particles have infinite mass.
    if(totalInverseMass <= 0)
    {
        return;
    }

    //Next we need to calculate the impulse.
    real impulse = deltaVelocity / totalInverseMass;

    //Next we need to find the impulse force we need to apply per inverted mass.
    Vector3 impulsePerInverseMass = mContactNormal * impulse;

    //Finally we apply the impulse which are applied in the directions of the contact and are proportion to the inverse mass.
    mParticles[0]->SetVelocity(mParticles[0]->GetVelocity() + impulsePerInverseMass * mParticles[0]->GetInverseMass());
    if(mParticles[1])
    {
        //The other particle goes the other way :)
        mParticles[1]->SetVelocity(mParticles[1]->GetVelocity() + impulsePerInverseMass * -mParticles[1]->GetInverseMass());
    }
}

void ParticleContact::resolveInterpenetration(real Duration)
{
    //If there is no interpenetration we just skip
    if(mPenetration <= 0)
    {
        return;
    }

    //The away from the penetration is given by the mass of the object.
    real totalInverseMass = mParticles[0]->GetInverseMass();
    if(mParticles[1])
    {
        totalInverseMass += mParticles[1]->GetInverseMass();
    }

    //If all particles are infinite mass we just return.
    if(totalInverseMass <= 0)
    {
        return;
    }

    //Next we need of course need to find the total of resolution penetration per unit of inverse mass.
    Vector3 perUnitofInverseMass = mContactNormal * (mPenetration / totalInverseMass);

    //Next we calculate the movement of both objects.
    mParticleMovement[0] = perUnitofInverseMass * mParticles[0]->GetInverseMass();
    if(mParticles[1])
    {
        mParticleMovement[1] = perUnitofInverseMass * -mParticles[1]->GetInverseMass();
    }
    else
    {
        mParticleMovement[1].Clear();
    }

    //Finally we need to change the position of the objects when we have found out how much they need to move.
    mParticles[0]->SetPosition(mParticles[0]->GetPosition() + mParticleMovement[0]);
    if(mParticles[1])
    {
        mParticles[1]->SetPosition(mParticles[1]->GetPosition() + mParticleMovement[1]);
    }
}

ParticleContactResolver::ParticleContactResolver(unsigned iterations) : mIteration(iterations)
{
}

void ParticleContactResolver::SetIteration(unsigned iterations)
{
    ParticleContactResolver::mIteration = iterations;
}
/**
    This function uses quicksort to find the deepest interpenetration value. May not work.
    Remember that we need a vector pointer and start index value and the end index value.
*/
void ParticleContactResolver::ResolveContact(ParticleContact *contactArray, unsigned numContacts, real duration)
{
    unsigned i;
    mInterationUsed = 0;

    while(mInterationUsed < mIteration)
    {
        // Find the contact with the largest closing velocity;
        real Max = std::numeric_limits<real>::max();
        unsigned maxIndex = numContacts;
        for(i = 0; i < numContacts; i++)
        {
            real sepVel = contactArray[i].calculateSepVelocity();
            if(sepVel < Max && (sepVel < 0 || contactArray[i].mPenetration > 0))
            {
                Max = sepVel;
                maxIndex = i;
            }
        }

        // Do we have anything worth resolving?
        if (maxIndex == numContacts)
        {
            break;
        }

        // Resolve this contact
        contactArray[maxIndex].resolve(duration);

        // Update the interpenetrations for all particles
        Vector3 *Move = contactArray[maxIndex].mParticleMovement;
        for (i = 0; i < numContacts; i++)
        {
            if (contactArray[i].mParticles[0] == contactArray[maxIndex].mParticles[0])
            {
                contactArray[i].mPenetration -= Move[0] * contactArray[i].mContactNormal;
            }
            else if (contactArray[i].mParticles[0] == contactArray[maxIndex].mParticles[1])
            {
                contactArray[i].mPenetration -= Move[1] * contactArray[i].mContactNormal;
            }
            if (contactArray[i].mParticles[1])
            {
                if (contactArray[i].mParticles[1] == contactArray[maxIndex].mParticles[0])
                {
                    contactArray[i].mPenetration += Move[0] * contactArray[i].mContactNormal;
                }
                else if (contactArray[i].mParticles[1] == contactArray[maxIndex].mParticles[1])
                {
                    contactArray[i].mPenetration += Move[1] * contactArray[i].mContactNormal;
                }
            }
        }

     mInterationUsed++;

    }
}
