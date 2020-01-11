#ifndef PFGEN_H_INCLUDED
#define PFGEN_H_INCLUDED
#include <vector>

#include "particle.h"
#include "Core.h"

/**
This class need to improve the particle air break class and the Gravitational particle pull class needs to be added.
*/

namespace wind
{
    //What this class does is it adds difference forces to particles.
    //This is what as known as a force generator.
    class ParticleForceGenerator
    {
        public:
            //What this function does it is takes a particle and calculates and updates the force on the particle. This is a pure virtual function so every class that inherance this class they will have to use this function
            virtual void UpdateForce(Particle* particle, real Duration) = 0;
    };
    /**Currently I don't know enough about the graviation pull to different objects
    //Rather than having a Vector3 object to define gravity that will accelerate at a constant rate.
    //We can implement as part of the force generator.
    class ParticleGravityPull : public ParticleForceGenerator
    {
        private:
            //This will hold the acceleration due to gravity.
            Vector3 mGravity;

            //The other particle the particle will be attracted to.
            Particle* mOther;

            //The universal gravitation constant.
            real mGravitationConstant;
        public:

            //This function will create a generator with the given acceleration.
            ParticleGravity(const Vector3& grav);

            //This of applies the gravitation force given to a particle.
            virtual void UpdateForce(Particle* particle, real Duration);
            virtual void UpdateForce(Particle* CentrePartice, Particle* particle, real Duration);

    }
*/

    class ParticleGravity : public ParticleForceGenerator
    {
        private:
            Vector3 mGravity;
        public:
            ParticleGravity(const Vector3& gravity);
            virtual void UpdateForce(Particle* particle, real Duration);
    };

    //This class may need some work later. This class creates a up lifting force for the particles.
    class ParticleUplift : public ParticleForceGenerator
    {
        private:
            //First we have 2 vectors to compare with the origin vector and the up lifting force.
            Vector3 mOriginPoint;
            Vector3 mUplift;

        public:
            //The origin point of the lift and the lift vector have to passed in as vectors.
            ParticleUplift(const Vector3& origin, const Vector3& lift);

            //Once they have been in we apply the uplift to the particle passed.
            virtual void UpdateForce(Particle* particle, real Duration);
    };

    //This class will calculate the drag force on a particle.
    //The formula is drag = -P.^(k1|P.^| + k2|P.^|) p is the drag vector, k1 and k2 are called to drag coefficient.
    //The formula coefficients change depending on the speed of the particle. As the speed increases k2 will grow at a faster rate.
    class ParticleDrag : public ParticleForceGenerator
    {
        private:
            //Holds the velocity drag coeffificent.
            real k1;
            //Holds the velocity drag coeffificent squared.
            real k2;
        public:
            //Sets up stuff.
            ParticleDrag(real k1, real k2);

            //Applies the drags to a given particle.
            virtual void UpdateForce(Particle* particle, real Duration);
    };

    class ParticleAirbrake
    {
        public:
            ParticleAirbrake(real k1, real k2);

            virtual void UpdateForce(Particle* particle, real Duration);

            void SetAirbrakeOff();
            void SetAirbrakeOn();

            bool GetAirbrake() const;
        private:
            bool mAirbrakeState;

            real k1;
            real k2;
    };

    /**
      This class applies a spring force onto a particle.
    */
    class ParticleSpring : public ParticleForceGenerator
    {
        public:
            //Creates a new spring force.
            ParticleSpring(Particle* other, real springConstant, real restLength);

            //Applies the spring force to the particle in the argument.
            virtual void UpdateForce(Particle* particle, real duration);
        private:
            //We need to have 2 particles, one at one and and one at the other. This is the other particle.
            Particle* mOther;

            //This is the Hook's law constant.
            real mSpringConstant;

            //This hold the springs rest length.
            real mRestLength;
    };

    class ParticleAnchoredSpring : public ParticleForceGenerator
    {
        public:
            ParticleAnchoredSpring();
            ParticleAnchoredSpring(Vector3* anchor, real springConstant, real restLength);

            void Init(Vector3* anchor, real springConstant, real restLength);
            const Vector3* getAnchor() { return mAnchor; }

            virtual void UpdateForce(Particle* particle, real Duration);
        protected:
            Vector3* mAnchor;

            real mSpringConstant;

            real mRestLength;
    };

    /**
        What this class is going to be is generate a spring force only when extended and it assumes that the spring is connected to 2 objects.
    */
    class ParticleBungee : public ParticleForceGenerator
    {
        public:
            ParticleBungee(Particle* other, real springConstant, real restLength);

            virtual void UpdateForce(Particle* particle, real Duration);
        private:
            Particle* mOther;

            real mSpringConstant;

            real mRestLength;
    };


    /**
        A force generator that applies a buoyancy for a plan of liquid parallel tot he XZ
    */
    class ParticleBuoyancy : public ParticleForceGenerator
    {
        public:
            ParticleBuoyancy(real maxdepth, real volume, real waterheight, real liquiddensity = 1000.0f);

            virtual void UpdateForce(Particle* particle, real Duration);
        private:
            //This holds the data for the max depth of a particle before it generates the maximum buoyancy force.
            real mMaxDepth;

            //Hold the the volume of the object.
            real mVolume;

            //TJe height if the water plane above y = 0. The plane will be parallel to the XZ plane.
            real mWaterHeight;

            //This is the density of the liquid. Pure water has a density of 1000kg per cubic meter.
            real mLiquidDensity;
    };

    /**
        This class inheritances from ParticleAnchoredSpring because the code is very similar and we are using the spring constructor to set everything up.
        This avoids initialising data that we could simply reuse and it allows us to simply just call the particle update function of the AnchorBungee class to apply the force.
    */
    class ParticleArchoredBungee : public ParticleAnchoredSpring
    {
        public:
            virtual void UpdateForce(Particle* particle, real Duration);
    };

    class ParticleFakeSpring : public ParticleForceGenerator
    {
        public:
            ParticleFakeSpring(Vector3* anchor, real springconst, real damping);

            virtual void UpdateForce(Particle* particle, real Duration);
        private:
            //This is the location of the anchored end of the spring.
            Vector3 *mAnchor;

            //Hold the spring constant.
            real mSpringConstant;

            //Holds the damping affect of the oscillation.
            real mDamping;
    };

    //When building a Particle Force Generator you need to designed something to store away what force has been applied to what particle.
    //This is what this class does is it holds all the force generated by the force generator and stores them away, along with the particle it applies to.
    class ParticleForceRegistry
    {
        protected:

            //This struct will keep track of all the particle force generations.
            struct ParticleForceRegistration
            {
                Particle* mParticles;
                ParticleForceGenerator* mPFG;
            };

            //This is a type defined vector that makes a type of a ParticleForceRegistration.
            //This will hold a list of the ParticleForceRegistration in a vector.
            typedef std::vector<ParticleForceRegistration> Registry;
            //Of course when we define a type we have to give it a instance.
            Registry mRegistration;
        public:
            //Registers a force need to be added to a particle.
            void Add(Particle* particle, ParticleForceGenerator* PFG);
            //Removes the registered particle and the force generator.
            //Note: That if the particle force generator and a particle are not linked this will have no affect.
            void Remove(Particle* particle, ParticleForceGenerator* PFG);
            //Clears only the register of the particles and there generator.
            //Note: This will not delete the particle or the particle force generator just the registration of the two.
            void Clear();
            //This function calls the force generator to updates the force of the corresponding particle.
            void UpdateForce(real duration);
    };
}
#endif // PFGEN_H_INCLUDED
