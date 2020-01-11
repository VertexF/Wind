#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

#include <assert.h>

#include "Core.h"
#include "precision.h"

namespace wind
{
    //This is a particle systems, which handles the simplest thing in physics.
    class Particle
    {
        public:

            void Intergrate(real duration);

            //These two function are the setter and getter function of mass.
            void SetMass(const real Mass);
            real GetMass() const;

            //This function sets up the inversion and returns is value.
            void SetInverseMass(const real InverseMass);
            real GetInverseMass() const;

            //This function returns true if the mass of an a particle is not infinite
            bool isNotInfinite() const;

            //This function applies the dampening affect on the particles and returns the damping value.
            void SetDamping(const real damp);
            real GetDamping() const;

            /**Sets up the position Vector*/
            //This function set the position of a particle by passing a reference vector.
            void SetPosition(const Vector3& Pos);

            //This overloads the function at set the position of a particle by floats.
            void SetPosition(const real x, const real y, const real z);

            //This changes the value of a non-const pointer pass in the function
            void GetPosition(Vector3* Position) const;

            //This function returns the current value of the position vector.
            Vector3 GetPosition() const;

            /**Sets up the velocity Vector*/
            //This function set the velocity of a particle by passing a reference vector.
            void SetVelocity(const Vector3& Pos);

            //This overloads the function at set the velocity of a particle by floats.
            void SetVelocity(const real x, const real y, const real z);

            //This changes the value of a non-const pointer pass in the function
            void GetVelocity(Vector3* Velocity) const;

            //This function returns the current value of the velocity vector.
            Vector3 GetVelocity() const;

            /**Sets up the acceleration Vector*/
            //This function set the acceleration of a particle by passing a reference vector.
            void SetAcceleration(const Vector3& Acc);

            //This overloads the function at set the acceleration of a particle by floats.
            void SetAcceleration(const real x, const real y, const real z);

            //This changes the value of a non-const pointer pass in the function
            void GetAcceleration(Vector3* Acceleration) const;

            //This function returns the current value of the acceleration vector.
            Vector3 GetAcceleration() const;

            /** Additional force functions */
            //This function clears any force applied to a particle. This is called after each integration step.
            void ClearAccumulator();

            //This Adds force to the accumulated force, this will be applied after the next iteration.
            void AddForce(const Vector3& AddForce);

        protected:
            //This will hold the linear position in the world.
            Vector3 mPosition;

            //Holds the linear velocity of the particle in the world.
            Vector3 mVelocity;

            //This holds the acceleration of the particle. This is useful for setting up gravity or any other constant acceleration.
            Vector3 mAcceleration;

            //This vector holds the total accumulated force that is added to a vector. Cleared after to zero after each iteration of the integration
            Vector3 mAccumulatedForce;

            //Because the processor can not calculate something going at a 0constant velocity, we need to have a damping drag force to removed add energy.
            real mDamping;

            //This is is going to be used to give really world objects infinite amount of mass.
            //It does this by dividing and negative version of the mass so when we calculate mass it becomes unmoveable.
            real mInverseMass;
    };

}

#endif // PARTICLE_H_INCLUDED
