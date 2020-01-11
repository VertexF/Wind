#include "Random.h"

using namespace wind;

Random::Random()
{
    SeedRandom(0);
}

Random::Random(unsigned int seed)
{
    SeedRandom(seed);
}

unsigned int Random::rotl(unsigned int n, unsigned int r)
{
    return(n << r) | (n >> (32 - r));
}

unsigned int Random::rotr(unsigned int n, unsigned int r)
{
    return(n >> r) | (n << (32 - r));
}

void Random::SeedRandom(unsigned int seed)
{
    if(seed == 0)
    {
        seed = (unsigned)clock();
    }

    //This fills a buffer with some random numbers.
    for(unsigned int i = 0; i < 17; i++)
    {
        //Now because this is unsigned and this number is huge and it clocks over to get more random numbers.
        seed = (seed * 2891336453) + 1;
        //Assign the random numbers to the buffer.
        mBuffer[i] = seed;
    }

    //Initiation of the buffer pointers.
    p1 = 0; p2 = 10;
}

unsigned int Random::RandomBit()
{
    unsigned int result;

    //Rotates the buffer and resigns it into it's self.
    result = mBuffer[p1] = rotl(mBuffer[p2], 13) + rotr(mBuffer[p1], 9);

    //This rotates the pointers.
    if(--p1 < 0)
    {
        p1 = 16;
    }

    if(--p2 < 0)
    {
        p2 = 16;
    }

    //Finally we return the result. Which is a random number.
    return result;
}

#ifdef SIGNLE_PRECISION
real Random::RandomFloat()
{
    //This generates a random number.
    unsigned int Bits = RandomBit();

    //This sets up a union for manipulation.
    union
    {
        real Value;
        unsigned int Word;
    } convert; //Note the object of this union is called convert.

    // Now assign the bits to the word. This works by fixing the ieee
    // sign and exponent bits (so that the size of the result is 1-2)
    // and using the bits to create the fraction part of the float.
    convert.Word = (Bit >> 9) | 0x3F800000;

    return (convert.Value - 1.0f);
}
#else
real Random::RandomFloat()
{
    unsigned int Bits = RandomBit();

    union
    {
        real Value;
        unsigned int Words[2];
    }convert;

    //First we assign the bits to Words. ieee sign(which is something to do with floats) and exponent bits.
    //This makes the size between 1 and 2.
    convert.Words[0] = Bits << 20; // Fill in the top 16 bits
    convert.Words[1] = (Bits >> 12) | 0x3FF00000; // And the bottom 20

    return (convert.Value - 1.0);
}
#endif

real Random::RandomFloat(real Min, real Max)
{
    return RandomFloat() * (Max - Min) + Min;
}

real Random::RandomFloat(real scale)
{
    return RandomFloat() * scale;
}

unsigned int Random::RandomInt(unsigned int Max)
{
    return RandomBit() % Max;
}

real Random::RandomBinomial(real scale)
{
    return(RandomFloat() - RandomFloat()) * scale;
}

Vector3 Random::RandomVector(const Vector3& scale)
{
    return Vector3(RandomBinomial(scale.x), RandomBinomial(scale.y), RandomBinomial(scale.z));
}

Vector3 Random::RandomVector(const real& scale)
{
    return Vector3(RandomBinomial(scale), RandomBinomial(scale), RandomBinomial(scale));
}

Quaternion Random::randomQuaternion()
{
	Quaternion q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
	q.normalise();

	return q;
}

Vector3 Random::RandomVector(const Vector3& Min, const Vector3& Max)
{
    return Vector3(RandomFloat(Min.x, Max.x), RandomFloat(Min.y, Max.y), RandomFloat(Min.z, Max.z));
}

Vector3 Random::RandomXZVector(real scalar)
{
    return Vector3(RandomBinomial(scalar), 0, RandomBinomial(scalar));
}
