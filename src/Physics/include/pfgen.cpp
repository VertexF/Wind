#include "pfgen.h"

using namespace wind;

void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator* PFG)
{
    //Here we make an object that of the nested struct ParticleForceRegistration.
    ParticleForceRegistry::ParticleForceRegistration reg;
    //Next we set everything equal to the nested struct to what we pass in.
    reg.mParticles = particle;
    reg.mPFG = PFG;
    //Finally we push_back a space in the vector.
    mRegistration.push_back(reg);
}

//This function implementation is a little ruff. Also I have no way of testing until the application is built.
//First you pass in the particle that you want to remove registration because the way this vector built it will delete the data as well as the index.
//This is good because the force generator and particle have to be passed in so when they are deleted the orginal particle it's self wont be delete.
void ParticleForceRegistry::Remove(Particle* particle, ParticleForceGenerator* PFG)
{
    //First we set up a simple for loop to go through every element in the vector.
    for(unsigned int i = 0; i < mRegistration.size(); i++)
    {
        //Next we test with the particle pasted if it is the same particle that's the same in the vector.
        //We do the same with the ParticleForceGenerator instance.
        if(mRegistration.at(i).mParticles == particle && mRegistration.at(i).mPFG == PFG)
            //Finally we only erase that data and the element that we need to.
            mRegistration.erase(mRegistration.begin() + i);
    }
}

void ParticleForceRegistry::Clear()
{
    mRegistration.clear();
}

void ParticleForceRegistry::UpdateForce(real duration)
{
    //First we make a for loop throw all the elements in the vector.
    for(Registry::iterator i = mRegistration.begin(); i != mRegistration.end(); i++)
    {
        //Second we get inside the iterator and use the ParticleForceGenerator to access the updates force function.
        //Which is the pure virtual function so, of course it will operate at its most derived state.
        i->mPFG->UpdateForce(i->mParticles, duration);
    }
}

/**
ParticleGravity::ParticleGravity(const Vector3& grav, Particle* other) :
mGravity(grav),
mOther(other),
mmGravitationConstant(9.8)
{
}


void ParticleGravity::UpdateForce(Particle* particle, real Duration)
{
    //First we check to see if the function doesn't have infinite mass if so, return.
    if(!particle->isNotInfinite())
    {
        return;
    }

    Vector3 force;
    force.GetPosition(&force);
    force ?? ?mGravity;

    mOther.magnitude();
    particle.magnitude();

    //Finally we add force to the give particle which is the gravity force multiply by the mass of the particle.
    particle->AddForce(mGravity * particle->GetMass());
}
*/

ParticleGravity::ParticleGravity(const Vector3& gravity) : mGravity(gravity)
{
}

void ParticleGravity::UpdateForce(Particle* particle, real Duration)
{
    if(!particle->isNotInfinite())
    {
       return;
    }

    particle->AddForce(mGravity * particle->GetMass());
}

ParticleUplift::ParticleUplift(const Vector3& origin, const Vector3& lift) : mOriginPoint(origin), mUplift(lift)
{
}

void ParticleUplift::UpdateForce(Particle* particle, real Duration)
{
    //First we check to see if the function doesn't have infinite mass if so, return.
    if(!particle->isNotInfinite())
    {
        return;
    }

    //Here we first 20.0f pixel sizes to the origin of the lift.
    //Next we see if that force is bigger or equal to the current position vector of the particle we will apply the force.
    //Otherwise do nothing.
    if((mOriginPoint.y + 20.0f) >= particle->GetPosition().y)
    {
        //First we make a temporary force vector and set it equal to the uplift vector.
        Vector3 force = mUplift;

        //This works just like the gravitation force so it will work.
        //This checks to see if the uplift is able to lift up with a particle, if the mass is to great it shouldn't move.
        if(force.y - particle->GetMass() > 0.0f)
        {
            //Next we change the uplift value to be less depending on the mass of the particle.
            force.y = force.y - particle->GetMass();

            //Finally we add this force.
            particle->AddForce(force);
        }
    }
}

ParticleDrag::ParticleDrag(real k1, real k2) : k1(k1), k2(k2)
{
}

void ParticleDrag::UpdateForce(Particle* particle, real Duration)
{
    //This holds the force we are going to apply to the particle.
    Vector3 force;
    //Here we set up the Vector to have the same velocity as the particle.
    particle->GetVelocity(&force);


    //Here we are calculating the total drag coefficient.
    real dragCoeff = force.magnitude();
    //Here we use the coefficient to make the drag formula that will be used to calculate the drag.
    dragCoeff = (k1 * dragCoeff) + (k2 * dragCoeff * dragCoeff);

    //Finally after working out the drag force we apply it.
    force.normalise();
    force *= -dragCoeff;
    particle->AddForce(force);
}

ParticleAirbrake::ParticleAirbrake(real k1, real k2) : mAirbrakeState(false), k1(k1), k2(k2)
{
}

void ParticleAirbrake::UpdateForce(Particle* particle, real Duration)
{
    //This holds the force we are going to apply to the particle.
    Vector3 force;
    //Here we set up the Vector to have the same velocity as the particle.
    particle->GetVelocity(&force);


    //Here we are calculating the total drag coefficient.
    real dragCoeff = force.magnitude();
    //Here we use the coefficient to make the drag formula that will be used to calculate the drag.
    if(mAirbrakeState)
    {
        dragCoeff = (k1 * dragCoeff) + (k2 * dragCoeff * dragCoeff * 2);
    }
    else
    {
        dragCoeff = (k1 * dragCoeff) + (k2 * dragCoeff * dragCoeff);
    }

    //Finally after working out the drag force we apply it.
    force.normalise();
    force *= -dragCoeff;
    particle->AddForce(force);
}

void ParticleAirbrake::SetAirbrakeOff()
{
    mAirbrakeState = false;
}

void ParticleAirbrake::SetAirbrakeOn()
{
    mAirbrakeState = true;
}

bool ParticleAirbrake::GetAirbrake() const
{
    return mAirbrakeState;
}

ParticleSpring::ParticleSpring(Particle* other, real springConstant, real restLength) :
    mOther(other),
    mSpringConstant(springConstant),
    mRestLength(restLength)
{
}

void ParticleSpring::UpdateForce(Particle* particle, real duration)
{
    //First we calculate the vector of the spring.
    Vector3 force;
    particle->GetPosition(&force);
    force -= mOther->GetPosition();

    //Next we calculate the magnitude of the force.
    real magnitude = force.magnitude();
    magnitude = std::abs(magnitude - mRestLength);
    magnitude *= mSpringConstant;

    //This calculates the final force and applies it.
    force.normalise();
    force *= -magnitude;
    particle->AddForce(force);
}

ParticleAnchoredSpring::ParticleAnchoredSpring()
{
}

ParticleAnchoredSpring::ParticleAnchoredSpring(Vector3* anchor, real springConstant, real restLength) :
    mAnchor(anchor),
    mSpringConstant(springConstant),
    mRestLength(restLength)
{
}

void ParticleAnchoredSpring::Init(Vector3* anchor, real springConstant, real restLength)
{
    this->mAnchor = anchor;
    this->mSpringConstant = springConstant;
    this->mRestLength = restLength;
}

void ParticleAnchoredSpring::UpdateForce(Particle* particle, real Duration)
{
        //First we calculate the vector of the spring.
    Vector3 force;
    particle->GetPosition(&force);
    force -= *mAnchor;

    //Next we calculate the magnitude of the force.
    real magnitude = force.magnitude();
    magnitude = (mRestLength - magnitude) * mSpringConstant;;

    //This calculates the final force and applies it.
    force.normalise();
    force *= magnitude;
    particle->AddForce(force);
}

ParticleBungee::ParticleBungee(Particle* other, real springConstant, real restLength) :
    mOther(other),
    mSpringConstant(springConstant),
    mRestLength(restLength)
{
}

void ParticleBungee::UpdateForce(Particle* particle, real Duration)
{
    //First we need to calculate the springs vector.
    Vector3 force;
    mOther->GetPosition(&force);
    force -= mOther->GetPosition();

    //Next we check to see if the force on the vector is less than the length if so we return.
    real magnitude = force.magnitude();
    if(magnitude <= mRestLength)
    {
        return;
    }

    //Next we calculate the magnitude of the force.
    magnitude = mSpringConstant * (mRestLength - magnitude);

    //Finally we calculate the final force and apply it.
    force.normalise();
    force *= -magnitude;
    particle->AddForce(force);
}

ParticleBuoyancy::ParticleBuoyancy(real maxdepth, real volume, real waterheight, real liquiddensity) :
    mMaxDepth(maxdepth),
    mVolume(volume),
    mWaterHeight(waterheight),
    mLiquidDensity(liquiddensity)
{
}

void ParticleBuoyancy::UpdateForce(Particle* particle, real Duration)
{
    //This calculates the submersion depth.
    real depth = particle->GetPosition().y;

    //Check if we're out of the water.
    if(depth >= mWaterHeight + mMaxDepth)
    {
        return;
    }
    Vector3 force(0, 0, 0);

    //Check if we are at the maximum depth.
    if(depth <= mWaterHeight - mMaxDepth)
    {
        force.y = mLiquidDensity * mVolume;
        particle->AddForce(force);
        return;
    }

    //This calculates the buoyancy force if we are only party submerged.
    force.y = mLiquidDensity * mVolume * (depth - mMaxDepth - mWaterHeight) / 2 * mMaxDepth;
    particle->AddForce(force);
}

void ParticleArchoredBungee::UpdateForce(Particle* particle, real Duration)
{
    //First we need to calculate the springs vector.
    Vector3 force;
    particle->GetPosition(&force);
    force -= *mAnchor;

    //Next we check to see if the force on the vector is less than the length if so we return.
    real magnitude = force.magnitude();
    if(magnitude < mRestLength)
    {
        return;
    }

    //Next we calculate the magnitude of the force.
    magnitude = magnitude - mRestLength;
    magnitude *= mSpringConstant;

    //Finally we calculate the final force and apply it.
    force.normalise();
    force *= -magnitude;
    particle->AddForce(force);
}

ParticleFakeSpring::ParticleFakeSpring(Vector3* anchor, real springconst, real damping) :
    mAnchor(anchor),
    mSpringConstant(springconst),
    mDamping(damping)
{
}

void ParticleFakeSpring::UpdateForce(Particle* particle, real Duration)
{
    //First we need to check to see if the particle has infinite mass if so we just return.
    if(!particle->isNotInfinite())
    {
        return;
    }

    //Then we calculate the relative position of the position of the anchor point.
    Vector3 position;
    particle->GetPosition(&position);
    position -= *mAnchor;

    //Next we get the Y constant also known as gamma and check it is in bounds.
    real gamma = 0.5f * std::sqrt(4 * mSpringConstant - mDamping * mDamping);
    if(gamma == 0.0f)
    {
        return;
    }

    //This is the C constant in the formula.
    Vector3 c = position * (mDamping / (2.0f * gamma)) + particle->GetVelocity() * (1.0f / gamma);

    //Here we are calculating the target position. This is the differential equation that will give the position at any time in the future.
    Vector3 target = position * std::cos(gamma * Duration) + c * std::sin(gamma * Duration);
    target *= std::exp(-0.5f * mDamping * mDamping);

    //Finally we work out the resulting force by getting the acceleration and the force we need to apply.
    Vector3 accel = (target - position) * (real(1.0f) / (Duration * Duration)) - particle->GetVelocity() * Duration;
    particle->AddForce(accel * particle->GetMass());
}
