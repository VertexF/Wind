#include "plinks.h"

using namespace wind;

real ParticleLink::currentLength() const
{
    Vector3 relativePosition = mParticles[0]->GetPosition() - mParticles[1]->GetPosition();
    return relativePosition.magnitude();
}

unsigned ParticleCable::addContact(ParticleContact* contact, unsigned limit) const
{
    //First we need to get the length of the
    real length = currentLength();

    //If we are not over extended then we move on.
    if(length < mMaxLength)
    {
        return 0;
    }

    //Otherwise we need to return then contacts(from the arguments) of the particles.
    contact->mParticles[0] = mParticles[0];
    contact->mParticles[1] = mParticles[1];

    //next we calculate the reserve normal.
    Vector3 normal = mParticles[1]->GetPosition() - mParticles[0]->GetPosition();
    normal.normalise();
    contact->mContactNormal = normal;

    contact->mPenetration = length - mMaxLength;
    contact->mRestitution = mRestitution;

    return 1;
}

unsigned ParticleRod::addContact(ParticleContact* contact, unsigned limit) const
{
    //First we need to get the length of the
    real curLength = currentLength();

    //If we are not over extended then we move on.
    if(curLength == mLength)
    {
        return 0;
    }

        //Otherwise we need to return then contacts(from the arguments) of the particles.
    contact->mParticles[0] = mParticles[0];
    contact->mParticles[1] = mParticles[1];

    //next we calculate the reserve normal.
    Vector3 normal = mParticles[1]->GetPosition() - mParticles[0]->GetPosition();
    normal.normalise();

    //The contact normal depends on whether we're are extending or compressing.
    if(curLength > mLength)
    {
        contact->mContactNormal = normal;
        contact->mPenetration = curLength - mLength;
    }
    else
    {
        contact->mContactNormal = normal * -1;
        contact->mPenetration = mLength - curLength;
    }

    //Always zero son.
    contact->mRestitution = 0;

    return 1;
}

real ParticleConstraint::currentLength() const
{
    Vector3 relativePos = mParticle->GetPosition() - mAnchor;

    return relativePos.magnitude();
}

unsigned ParticleCableConstraint::addContact(ParticleContact* contact, unsigned limit) const
{
    //First we need to get the length of the
    real length = currentLength();

    //If we are not over extended then we move on.
    if(length < mMaxLength)
    {
        return 0;
    }

    //Otherwise we need to return then contacts(from the arguments) of the particles.
    contact->mParticles[0] = mParticle;
    contact->mParticles[1] = 0;

    //next we calculate the reserve normal.
    Vector3 normal = mAnchor - mParticle->GetPosition();
    normal.normalise();
    contact->mContactNormal = normal;

    contact->mPenetration = length - mMaxLength;
    contact->mRestitution = mRestitution;

    return 1;
}

unsigned ParticleRodConstraint::addContact(ParticleContact* contact, unsigned limit) const
{
//First we need to get the length of the
    real curLength = currentLength();

    //If we are not over extended then we move on.
    if(curLength == mLength)
    {
        return 0;
    }

        //Otherwise we need to return then contacts(from the arguments) of the particles.
    contact->mParticles[0] = mParticle;
    contact->mParticles[1] = 0;

    //next we calculate the reserve normal.
    Vector3 normal = mAnchor - mParticle->GetPosition();
    normal.normalise();

    //The contact normal depends on whether we're are extending or compressing.
    if(curLength > mLength)
    {
        contact->mContactNormal = normal;
        contact->mPenetration = curLength - mLength;
    }
    else
    {
        contact->mContactNormal = normal * -1;
        contact->mPenetration = mLength - curLength;
    }

    //Always zero son.
    contact->mRestitution = 0;

    return 1;
}
