#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include <ctime>
#include <cstdlib>

#include "core.h"

namespace wind
{
    //This class will keep track of random seeds such as seeds and the outputs.
    //This class is useful because we can have multiply random streams happening at the same time as each other.
    class Random
    {
        public:
            //This function creates a random right bitwise rotation.
            unsigned int rotl(unsigned int n, unsigned int r);

            //This function creates a random left bitwise rotation.
            unsigned int rotr(unsigned int n, unsigned int r);

            //The constructor creates a new random number with the seed based off time.
            Random();

            //Creates a new random number with a given seed.
            Random(unsigned int seed);

            //This gets a new number with a seed.
            void SeedRandom(unsigned int seed);

            //This function creates a random bitstring from a string.
            //Note: This is fast.
            unsigned int RandomBit();

            //This function gets a random float between 0 and 1.
            real RandomFloat();

            //This function gets a random float between the min and max value.
            real RandomFloat(real Min, real Max);

            real RandomFloat(real scale);

            //Returns a random int below the number passed in the arguments.
            unsigned int RandomInt(unsigned int Max);

            //Returns a random binomially distributed number between -scale and +scale.
            //TODO: If you do not learn what this is in your physic study then learn outside of physics.
            real RandomBinomial(real scale);

            //This makes, a random vector and the new vector  is binomially distributed in the range (-scale to scale)
            Vector3 RandomVector(const Vector3& scale);

			 //This function uses a scalar to make a random vector with the random binomial.
			Vector3 RandomVector(const real& scale);

            //This produces a random vector, that is contained inside a cube.
            Vector3 RandomVector(const Vector3& Min, const Vector3& Max);

			//This function randomly generates a random quaternion and normalises it so it is useful.
			Quaternion randomQuaternion();

            //Creates and returns a Vector with random x and z coordinates.
            Vector3 RandomXZVector(real scalar);

        private:

           int p1, p2;
           unsigned mBuffer[17];
    };
}

#endif // RANDOM_H_INCLUDED
