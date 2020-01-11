#ifndef PRECISION_H
#define PRECISION_H

#include <float.h>

//We are of course using this so we don't class with other names.
namespace wind
{
    /** Defines the highest value for the real number. */
    #define R_PI 3.141592653589793

	#define real_epsilon 2.2204460492503131e-16
    //This is used so if we need to change the data type set up inside the core of the physic engine we can just change this defended type instead aload of data types.
    using real = double;
}
#endif // PRECISION_H
