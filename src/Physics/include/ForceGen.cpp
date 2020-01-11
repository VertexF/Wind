#include "ForceGen.h"

using namespace wind;

    Gravity::Gravity(const Vector3 &grav) : gravity(grav)
    {
    }

    void Gravity::updateForce(RigidBody *body, real duration)
    {
        if(body->hasInfiniteMass())
        {
            return;
        }

        body->addForce(gravity * body->getMass());
    }

    Propulsion::Propulsion(const Vector3 &prop, const Vector3 &pos, const Vector3 *wind) :
        windspeed(wind),
        position(pos),
        propulsion(prop)
    {
    }

    void Propulsion::updateForce(RigidBody *body, real duration)
    {
        if(body->hasInfiniteMass())
        {
            return;
        }

        //First we need to calculate the velocity of the wind and the body.
        propulsion += *windspeed;

        Vector3 bodyVelocity = body->getTransform().transformDirection(propulsion);

        Vector3 bodyForce = bodyVelocity * body->getInverseMass();

        body->addForce(bodyForce);

    }

    Spring::Spring(const Vector3 &localConnection, RigidBody *other, const Vector3 &otherConnection, real springConst, real restLen) :
        connectionPoint(localConnection),
        other(other),
        otherConnectionPoint(otherConnection),
        springConstant(springConst),
        restLength(restLen)
    {
    }

    void Spring::updateForce(RigidBody *body, real duration)
    {
        //First we need to calculate the points in world space.
        Vector3 lws = body->getPointInWorld(connectionPoint);
        Vector3 ows = other->getPointInWorld(otherConnectionPoint);

        //Next we simply calculate the force of the spring.
        Vector3 force = lws - ows;

        //Next we calculate the magnitude of the force with the rest length and the spring constant of the spring force.
        real magnitude = force.magnitude();
        magnitude = std::abs(magnitude - restLength);
        magnitude *= springConstant;

        //Finally we normalise it and use the magnitude and add the force.
        force.normalise();
        force *= -magnitude;
        body->addForceAtPoint(force, lws);
    }

    Buoyancy::Buoyancy(const Vector3& cOfB, real maxDepth, real volume, real waterHeight, real liquidDensity)
    {
		centreOfBuoyancy = cOfB;
        this->maxDepth = maxDepth;
        this->volume = volume;
        this->waterHeight = waterHeight;
        this->liquidDensity = liquidDensity;
    }

    void Buoyancy::updateForce(RigidBody* body, real duration)
    {
		//Firstly we need to get the find where the water lays.
		Vector3 PointInWorld = body->getPointInWorld(centreOfBuoyancy);
		real depth = PointInWorld.y;

   		 //Next we need to check to see
   		 if (depth >= waterHeight + maxDepth)
		 {
			return;
		 }

		 Vector3 force(0, 0, 0);

   		 // Check if we're at maximum depth
   		 if (depth <= waterHeight - maxDepth)
    	 {
      		force.y = liquidDensity * volume;
       		body->addForceAtBodyPoint(force, centreOfBuoyancy);
        	return;
   		 }

   		// Otherwise we are partly submerged
    	force.y = liquidDensity * volume * (depth - maxDepth - waterHeight) / 2 * maxDepth;
    	body->addForceAtBodyPoint(force, centreOfBuoyancy);
    }

    Aero::Aero(const Matrix3 &ten, const Vector3 &pos, const Vector3 *wind) :
        Tensor(ten),
        position(pos),
        windspeed(wind)
    {
    }

    void Aero::updateForce(RigidBody *body, real duration)
    {
        updateForceFromTensor(body, duration, Tensor);
    }

    void Aero::updateForceFromTensor(RigidBody *body, real duration, const Matrix3 &tensor)
    {

        //First we need to calculate the velocity of the wind and the body.
        Vector3 velocity = body->getVelocity();
        velocity += *windspeed;

        //Next we need to calculate the velocity in the bodies local space.
        //Try to remember how this is done, it's important.
        Vector3 bodyVel = body->getTransform().transformInverseDirection(velocity);

        //Next we need to calculate the force that is being applied to the body.
        Vector3 bodyForce = tensor.transform(bodyVel);
        Vector3 force = body->getTransform().transformDirection(bodyForce);

        //Finally we add the force to the body
        body->addForceAtBodyPoint(force, position);
    }

    AeroControl::AeroControl(const Matrix3 &base, const Matrix3 &min, const Matrix3 &max, const Vector3 &pos, const Vector3 *wind) :
        Aero(base, pos, wind),
        maxTensor(max),
        minTensor(min),
        controlSettings(0)
    {
    }

    Matrix3 AeroControl::getTensor()
    {
        if(controlSettings <= -1)
        {
            return minTensor;
        }
        else if(controlSettings >= 1)
        {
            return maxTensor;
        }
        else if(controlSettings < 0)
        {
            return Matrix3::linearInterpolate(minTensor, Tensor, controlSettings + 1.f);
        }
        else if(controlSettings > 0)
        {
            return Matrix3::linearInterpolate(Tensor, maxTensor, controlSettings);
        }
        else
        {
            return Tensor;
        }
    }

    void AeroControl::setControl(real value)
    {
        controlSettings = value;
    }

    void AeroControl::updateForce(RigidBody *body, real duration)
    {
        Matrix3 tensor = getTensor();
        Aero::updateForceFromTensor(body, duration, tensor);
    }

    void ForceRegistry::add(RigidBody* body, ForceGenerator* FG)
    {
        //Here we make an object that of the nested struct ForceRegistration.
        ForceRegistry::ForceRegistration reg;
        //Next we set everything equal to the nested struct to what we pass in.
        reg.mBody = body;
        reg.mFG = FG;
        //Finally we push_back a space in the vector.
        mRegistration.push_back(reg);
    }

    void ForceRegistry::remove(RigidBody* body, ForceGenerator* FG)
    {
        //First we set up a simple for loop to go through every element in the vector.
        for(unsigned int i = 0; i < mRegistration.size(); i++)
        {
            //Next we test with the rigid body pasted if it is the same rigid body that's the same in the vector.
            //We do the same with the ForceGenerator instance.
            if(mRegistration.at(i).mBody == body && mRegistration.at(i).mFG == FG)
                //Finally we only erase that data and the element that we need to.
                mRegistration.erase(mRegistration.begin() + i);
        }
    }

    void ForceRegistry::clear()
    {
        mRegistration.clear();
    }

    void ForceRegistry::updateForce(real duration, RigidBody* body)
    {
        //First we make a for loop throw all the elements in the vector.
        for(Registry::iterator i = mRegistration.begin(); i != mRegistration.end(); i++)
        {
            //Second we get inside the iterator and use the ForceGenerator to access the updates force function.
            //Which is the pure virtual function so, of course it will operate at its most derived state.
            i->mFG->updateForce(i->mBody, duration);
        }
    }
