#include "particle.h"

using namespace wind;

void Particle::Intergrate(real duration)
{
    if(mInverseMass <= 0.f)
    {
        return;
    }

    assert(duration > 0.0);

    //Update linear position of the an object
    mPosition.addScaledVector(mVelocity, duration);

    //Works out the acceleration from a force.
    //TODO: Late add another vector to this when we need to generate force.
    Vector3 resultAcc = mAcceleration;
    resultAcc.addScaledVector(mAccumulatedForce, mInverseMass);

    //This get the linear acceleration and adds that to the velocity.
    mVelocity.addScaledVector(resultAcc, duration);

    //This imposes the drag on the on the velocity vector.
    mVelocity *= std::pow(mDamping, duration);

    //Clear the force.
    ClearAccumulator();
}

void Particle::SetMass(const real Mass)
{
    assert(Mass != 0);
    //This is how we invert the mass when calculating the mass of a particle.
    mInverseMass = ((real)1.0 ) / Mass;
}

real Particle::GetMass() const
{
    if(mInverseMass == 0)
    {
        //If the inverse mass is = 0 we return the highest number we can.
        return std::numeric_limits<real>::max();
    }
    else
    {
        //If inverse mass is not equal to zero then we return the mass divided by 1 to get how much it will be able to accelerate by.
        return ((real)1.0) / mInverseMass;
    }
}

void Particle::SetInverseMass(const real InverseMass)
{
    mInverseMass = InverseMass;
}

real Particle::GetInverseMass() const
{
    return mInverseMass;
}

bool Particle::isNotInfinite() const
{
    return (mInverseMass >= 0.0f);
}

void Particle::SetDamping(const real damp)
{
    mDamping = damp;
}

real Particle::GetDamping() const
{
    return mDamping;
}

void Particle::SetPosition(const Vector3& Pos)
{
    mPosition = Pos;
}

void Particle::SetPosition(const real x, const real y, const real z)
{
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;
}

void Particle::GetPosition(Vector3* Position) const
{
    *Position = mPosition;
}

Vector3 Particle::GetPosition() const
{
    return mPosition;
}

void Particle::SetVelocity(const Vector3& Vec)
{
    mVelocity = Vec;
}

void Particle::SetVelocity(const real x, const real y, const real z)
{
    mVelocity.x = x;
    mVelocity.y = y;
    mVelocity.z = z;
}

void Particle::GetVelocity(Vector3* Velocity) const
{
    *Velocity = mVelocity;
}

Vector3 Particle::GetVelocity() const
{
    return mVelocity;
}

void Particle::SetAcceleration(const Vector3& Acc)
{
    mAcceleration = Acc;
}

void Particle::SetAcceleration(const real x, const real y, const real z)
{
    mAcceleration.x = x;
    mAcceleration.y = y;
    mAcceleration.z = z;
}

void Particle::GetAcceleration(Vector3* Acceleration) const
{
    *Acceleration = mAcceleration;
}

Vector3 Particle::GetAcceleration() const
{
    return mAcceleration;
}

void Particle::ClearAccumulator()
{
    mAccumulatedForce.Clear();
}

void Particle::AddForce(const Vector3& AddForce)
{
    mAccumulatedForce += AddForce;
}
