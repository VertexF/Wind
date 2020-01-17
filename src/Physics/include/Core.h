#ifndef CORE_H
#define CORE_H
#include <iostream>
#include <assert.h>
#include <cmath>
#include <limits>

#include "precision.h"


namespace wind
{
    //Using a Singleton rather than an extern value.
    class EpsilonValue
    {
        public:
            EpsilonValue() = delete;
            EpsilonValue(const EpsilonValue& rhs) = delete;

            /**
             * Gets the current value of the sleep epsilon parameter.
             */
            static real& Epsilon();

            /**
             * Sets the current sleep epsilon value: the kinetic energy under
             * which a body may be put to sleep. Bodies are put to sleep if
             * they appear to have a stable kinetic energy less than this
             * value. This so they don't suddently stop.
             */
            void setSleepEpsilon(const real &value){ sleepEpsilon = value; }
        private:
            ~EpsilonValue() {}

            /**
             * Holds the value for energy under which a body will be put to
             * sleep. This will true for all body simulations.
             */
            static real sleepEpsilon;
    };


	/**
		Vector2 class has nothing to do with physics what so ever, it is purely here for the graphics.
	*/
	class Vector2
	{
		public:
			real x;
			real y;

            //Set all vectors to zero.
            Vector2() : x(0.0), y(0.0) {}
            //Sets up the vectors
            Vector2(const real x, const real y) : x(x), y(y){}

			real operator[](const unsigned int& i) const
            {
                if(i == 0)
                {
                    return x;
                }
                return y;
            }

            real& operator[](const unsigned int& i)
            {
                if(i == 0)
                {
                    return x;
                }
                return y;
            }

			//This overloader operator checks to see if two vectors are the same.
            bool operator==(const Vector2& rhs) const
            {
                return(x == rhs.x && y == rhs.y);
            }

			//Multiplies the vectors to give a scalar value.
            void operator*=(const real& value)
            {
                x *= value;
                y *= value;
            }

            //Returns a copy of the give in the arguments
            Vector2 operator*(const real& value) const
            {
                return Vector2(x * value, y * value);
            }

            //Works out the scalar product of a vector and returns it, with the operator.
            real operator*(const Vector2& vec) const
            {
                return (x * vec.x + y * vec.y);
            }

            //Adds to vectors together
            void operator+=(const Vector2& v)
            {
                x += v.x;
                y += v.y;
            }

            //Returns the added vectors together
            Vector2 operator+(const Vector2& v) const
            {
                return Vector2(x + v.x, y + v.y);
            }

            //subtracts to vectors together
            void operator-=(const Vector2& v)
            {
                x -= v.x;
                y -= v.y;
            }

            //Returns the subtracts vectors together
            Vector2 operator-(const Vector2& v) const
            {
                return Vector2(x - v.x, y - v.y);
            }

			//Uses operator == to test to see if both vectors unequal, if this is true it returns true.
            bool operator!=(const Vector2& rhs) const
            {
                return!(*this == rhs);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator<(const Vector2& rhs) const
            {
                return(x < rhs.x && y < rhs.y);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator>(const Vector2& rhs) const
            {
                return(x > rhs.x && y > rhs.y);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator<=(const Vector2& rhs) const
            {
                return(x <= rhs.x && y <= rhs.y);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator>=(const Vector2& rhs) const
            {
                return(x >= rhs.x && y >= rhs.y);
            }

            //This is not often talked about in mathematics and it doesn't yet have a operator that is constant to denote this type of vector operation.
            //Mainly this is not talked about because the product component doesn't have a geometric interpretation.
            //What this means is that if we multiply to vectors together represents new positions, then it isn't clear geometrically how the components product is related to the positions to their locations.
            //This function calculates and returns the wise-component product of this vector with the given vector.
            Vector2 componentProduct(const Vector2& vec) const
            {
                return Vector2(x * vec.x, y * vec.y);
            }

            //This function calculates and sets the wise-component product of this vector with the given vector to the vectors in this class.
            void componentProductUpdate(const Vector2& vec)
            {
                x *= vec.x;
                y *= vec.y;
            }

            //Works out the scalar product of a vector and returns it.
			//AKA dot product.
            real scalarProduct(const Vector2& vec)
            {
                return (x * vec.x + y * vec.y);
            }

            // Limits the size of the vector to the given maximum.
            void Trim(real Size)
            {
                if(squareMagnitude() > Size * Size)
                {
                    normalise();
                    x *= Size;
                    y *= Size;
                }
            }

            //This function inverts the vectors
            void invert()
            {
                x = -x;
                y = -y;
            }

            //This function clears all values
            void Clear()
            {
                x = y = 0;
            }

            //This function gets the magnitude of a vector, with a function.
            real magnitude() const
            {
                return std::sqrt(x * x + y * y);
            }

            //This gets the squared magnitude of the vector used for comparing vector magnitude.
            //This function is faster.
            real squareMagnitude() const
            {
                return (x * x + y * y);
            }

            //This function is used for adding vectors together and then scales it up.
            void addScaledVector(const Vector2& vec, real scale)
            {
                x += vec.x * scale;
                y += vec.y * scale;
            }

            //Turns a none-zero vector into normalises the vector basically gets the length of the vector.
            void normalise()
            {
                real l = magnitude();
                if(l > 0)
                {
                    (*this) *= (static_cast<real>(1)) / l;
                }
            }

            //This function returns the normal of a vector that this class is currently handling.
            Vector2 UnitVector()
            {
                Vector2 Result = *this;
                Result.normalise();
                return Result;
            }

			//This only works in 2D, it takes a degree value turns it into a radian and uses the rotation formula for to rotate the vector.
			Vector2 rotate(real degree)
			{
				real radian = degree * R_PI / 180.0;
				real rotcos = std::cos(radian);
				real rotsin = std::sin(radian);

				return Vector2((x * rotcos - y * rotsin), (x * rotsin + y * rotcos));
			}

	};

	class Quaternion;

    class Vector3
	{

        public:
            //The three vectors
            real x;
            real y;
            real z;

            //Set all vectors to zero.
            Vector3() : x(0.0), y(0.0), z(0.0), pad(1.0) {}
            //Sets up the vectors
            Vector3(const real x, const real y, const real z) : x(x), y(y), z(z), pad(1.0) {}

            //These hold static vector objects that will be used for different things in the engine.
            const static Vector3 GRAVITY;
            const static Vector3 HIGH_GRAVITY;
            const static Vector3 UP;
            const static Vector3 RIGHT;
            const static Vector3 OUT_OF_SCREEN;
            const static Vector3 X;
            const static Vector3 Y;
            const static Vector3 Z;

            real operator[](const unsigned int &i) const
            {
                if(i == 0)
                {
                    return x;
                }
                if(i == 1)
                {
                    return y;
                }
                return z;
            }

            real& operator[](const unsigned int &i)
            {
                if(i == 0)
                {
                    return x;
                }
                if(i == 1)
                {
                    return y;
                }
                return z;
            }

            //Multiplies the vectors to give a scalar value.
            void operator*=(const real &value)
            {
                x *= value;
                y *= value;
                z *= value;
            }

            //Returns a copy of the give in the arguments
            Vector3 operator*(const real& value) const
            {
                return Vector3(x * value, y * value, z * value);
            }

            //Works out the scalar product of a vector and returns it, with the operator.
            real operator*(const Vector3& vec) const
            {
                return (x * vec.x + y * vec.y + z * vec.z);
            }

			//Divides the vectors to give a scalar value.
            void operator/=(const real& value)
            {
				assert(value != 0);

                x /= value;
                y /= value;
				z /= value;
            }

            //Returns a copy of the give in the arguments
            Vector3 operator/(const real& value) const
            {
				assert(value != 0);

                return Vector3(x / value, y / value, z / value);
            }

            //Adds to vectors together
            void operator+=(const Vector3& v)
            {
                x += v.x;
                y += v.y;
                z += v.z;
            }

            //Returns the added vectors together
            Vector3 operator+(const Vector3& v) const
            {
                return Vector3(x + v.x, y + v.y, z + v.z);
            }

            //subtracts to vectors together
            void operator-=(const Vector3& v)
            {
                x -= v.x;
                y -= v.y;
                z -= v.z;
            }

            //Returns the subtracts vectors together
            Vector3 operator-(const Vector3& v) const
            {
                return Vector3(x - v.x, y - v.y, z - v.z);
            }

            //This function updates the vector to be the vector product of it's current value and given vector.
            //Here we have used the =% operator overloader to calculate the vector product and we have used the %= because it looks mostly like a X.
            void operator%=(const Vector3& vec)
            {
                *this = vectorProduct(vec);
            }

            //This is the operator that calculates the vector product of two vector in operator form.
            //The reason we have used the modulator overloader operator is simply looks like a cross.
            Vector3 operator%(const Vector3& vec) const
            {
                return Vector3(y * vec.z - z * vec.y,
                                z * vec.x - x * vec.z,
                                x * vec.y - y * vec.x);
            }

            //This overloader operator checks to see if two vectors are the same.
            bool operator==(const Vector3& rhs) const
            {
                return(x == rhs.x && y == rhs.y && z == rhs.z);
            }

            //Uses operator == to test to see if both vectors unequal, if this is true it returns true.
            bool operator!=(const Vector3& rhs) const
            {
                return!(*this == rhs);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator<(const Vector3& rhs) const
            {
                return(x < rhs.x && y < rhs.y && z < rhs.z);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator>(const Vector3& rhs) const
            {
                return(x > rhs.x && y > rhs.y && z > rhs.z);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator<=(const Vector3& rhs) const
            {
                return(x <= rhs.x && y <= rhs.y && z <= rhs.z);
            }

            //Returns true if all of the vectors values are the same as the arguments.
            bool operator>=(const Vector3& rhs) const
            {
                return(x >= rhs.x && y >= rhs.y && z >= rhs.z);
            }

            //This is not often talked about in mathematics and it doesn't yet have a operator that is constant to denote this type of vector operation.
            //Mainly this is not talked about because the product component doesn't have a geometric interpretation.
            //What this means is that if we multiply to vectors together represents new positions, then it isn't clear geometrically how the components product is related to the positions to their locations.
            //This function calculates and returns the wise-component product of this vector with the given vector.
            Vector3 componentProduct(const Vector3& vec) const
            {
                return Vector3(x * vec.x, y * vec.y, z * vec.z);
            }

            //This function calculates and sets the wise-component product of this vector with the given vector to the vectors in this class.
            void componentProductUpdate(const Vector3& vec)
            {
                x *= vec.x;
                y *= vec.y;
                z *= vec.z;
            }

            //Works out the scalar product of a vector and returns it.
            real scalarProduct(const Vector3& vec)
            {
                return (x * vec.x + y * vec.y + z * vec.z);
            }

            // Limits the size of the vector to the given maximum.
            void Trim(const real& Size)
            {
                if(squareMagnitude() > Size * Size)
                {
                    normalise();
                    x *= Size;
                    y *= Size;
                    z *= Size;
                }
            }

            //This function inverts the vectors
            void invert()
            {
                x = -x;
                y = -y;
                z = -z;
            }

            //This function clears all values
            void Clear()
            {
                x = y = z = 0;
            }

            //This function gets the magnitude of a vector, with a function.
            real magnitude() const
            {
                return std::sqrt(x * x + y * y + z * z);
            }

            //This gets the squared magnitude of the vector used for comparing vector magnitude.
            //This function is faster.
            real squareMagnitude() const
            {
                return (x * x + y * y + z * z);
            }

            //This function is used for adding vectors together and then scales it up.
            void addScaledVector(const Vector3& vec, const real& scale)
            {
                x += vec.x * scale;
                y += vec.y * scale;
                z += vec.z * scale;
            }

            //Turns a none-zero vector into normalises the vector basically gets the length of the vector.
            void normalise()
            {
                real l = magnitude();
                if(l > 0)
                {
                    (*this) *= static_cast<real>(1.0) / l;
                }
            }

            //This function returns the normal of a vector that this class is currently handling.
            Vector3 UnitVector()
            {
                Vector3 Result = *this;
                Result.normalise();
                return Result;
            }

            //This function works out the vector product of a vector passed in and the current vector object.
            //This return a new vector.
            Vector3 vectorProduct(const Vector3 &vec) const
            {
                //The mathematical syntax are a little odd this is how you calculate the new vector coordinates.
                return Vector3(y * vec.z - z * vec.y,
                                z * vec.x - x * vec.z,
                                x * vec.y - y * vec.x);
            }

            //This function makes the Orthonormal basis if you don't understand read about it in your vector paper.
            //When we start developing contact detection and contact resolution later, this algorithm will need to be updated.
            void makeOrthonormalBasis(Vector3 &a, Vector3 &b, Vector3 &c)
            {
                a.normalise();
                c = a % b;
                if(c.squareMagnitude() == 0.0)
                {
                    std::cerr << "Orthonormal Basis failed to complete: A and B are parallel vectors" << std::endl;
                    return;
                }
                c.normalise();
                b = c % a;
            }

            void makePoint()
            {
                if(pad == 1.0)
                {
                    pad = 0.0;
                }
            }

			Vector3 rotate(real degree, Vector3 axis);
			Vector3 rotate(Quaternion rotation);
			Vector3 rotate(Quaternion rotation, Vector3 vec);
			Vector3 moveForward(Quaternion rotation);
			Vector3 moveSidewards(Quaternion rotation);

        private:
            //This variable tells us if it's a vector or a point. 1 for vector which is default 0 for a point.
            real pad;
    };

     /**
        This class handles quaternion calculations and holds the data for the quaternion
    */
    class Quaternion
    {
        public:
            union
            {
                struct
                {
                    //Hold the real component of the quaternion
                    real r;

                    //Holds the first complex number in the quaternion
                    real i;

                    //Holds the second complex number in the quaternion
                    real j;

                    //Holds the third complex number in the quaternion
                    real k;
                };

                //Holds the quaternion in array form
                real data[4];
            };

            Quaternion() : r(1.0), i(0.0), j(0.0), k(0.0)
            {
            }

            Quaternion(real w, real x, real y, real z) : r(w), i(x), j(y), k(z)
            {
            }

			Quaternion(Vector3 axis, real angle)
			{
				real sinHalfAngle = std::sin((angle * R_PI/180) / 2);
				real cosHalfAngle = std::cos((angle * R_PI/180) / 2);

				i = axis.x * sinHalfAngle;
				j = axis.y * sinHalfAngle;
				k = axis.z * sinHalfAngle;
				r = cosHalfAngle;
			}

			//returns the real component of the quaternion
			real getR() const
			{
				return r;
			}

            //returns the first complex number in the quaternion
            real getI() const
			{
				return i;
			}

             //returns the second complex number in the quaternion
            real getJ() const
			{
				return j;
			}

            //returns the third complex number in the quaternion
            real getK() const
			{
				return k;
			}

            //This function normalises the quaternion making it valid for orientation.
            void normalise()
            {
                real d = r * r + i * i + j * j + k * k;

                //If this is zero then this is no rotation has happened.
                if(d < real_epsilon)
                {
                    r = 1;
                    return;
                }

				d = 1.0 / std::sqrt(d);

				r *= d;
				i *= d;
				j *= d;
				k *= d;
            }

            //This two multiplies quaternion together(This is the same process as any other)
            Quaternion operator*=(const Quaternion &multiplier)
            {
                Quaternion q = *this;

                r = q.r * multiplier.r - q.i * multiplier.i - q.j * multiplier.j - q.k * multiplier.k;
                i = q.r * multiplier.i + q.i * multiplier.r + q.j * multiplier.k - q.k * multiplier.j;
                j = q.r * multiplier.j + q.j * multiplier.r + q.k * multiplier.i - q.i * multiplier.k;
                k = q.r * multiplier.k + q.k * multiplier.r + q.i * multiplier.j - q.j * multiplier.i;

				return Quaternion(r, i, j, k);
            }

			Quaternion operator*(const Quaternion &multiplier)
            {
                Quaternion q = *this;

                real r = q.r * multiplier.r - q.i * multiplier.i - q.j * multiplier.j - q.k * multiplier.k;
                real i = q.r * multiplier.i + q.i * multiplier.r + q.j * multiplier.k - q.k * multiplier.j;
                real j = q.r * multiplier.j + q.j * multiplier.r + q.k * multiplier.i - q.i * multiplier.k;
                real k = q.r * multiplier.k + q.k * multiplier.r + q.i * multiplier.j - q.j * multiplier.i;

				return Quaternion(r, i, j, k);
            }

			Quaternion operator*(const Vector3 &multiplier)
            {
                Quaternion q = *this;

                real r = -q.i * multiplier.x - q.j * multiplier.y - q.k * multiplier.z;
                real i = q.r * multiplier.x + q.j * multiplier.z - q.k * multiplier.y;
                real j = q.r * multiplier.y + q.k * multiplier.x - q.i * multiplier.z;
                real k = q.r * multiplier.z + q.i * multiplier.y - q.j * multiplier.x;

				return Quaternion(r, i, j, k);
            }

            void rotateByVector(const Vector3 &vec)
            {
                Quaternion q(0, vec.x, vec.y, vec.z);
                (*this) *= q;
            }

            //This function adds the give vector which is scalar value this updates the orientation of the quaternion.
            void addScaledVector(const Vector3 &vec, real scalar)
            {
                Quaternion q(0, vec.x * scalar, vec.y * scalar, vec.z * scalar);
                q *= (*this);

                r += q.r * 0.5;
                i += q.i * 0.5;
                j += q.j * 0.5;
                k += q.k * 0.5;
            }

			Quaternion conjugate()
			{
				Quaternion q(r, -i, -j, -k);
				return q;
			}

			/*These fucntions are the camera rotation*/
			Vector3 getForward()
			{
				//return Vector3(2.0 * (i * k - r * j), 2.0 * (j * k + r * i), 1.0 - 2.0 * (i * i + j * j));
				return Vector3(0.0, 0.0, 1.0).rotate(*this);
			}

			Vector3 getBack()
			{
				//return Vector3(-2.0 * (i * k - r * j), -2.0 * (j * k + r * i), -(1.0 - 2.0 * (i * i + j * j)));
				return Vector3(0.0, 0.0, -1.0).rotate(*this);
			}

			Vector3 getUp()
			{
				//return Vector3(2.0 * (i * j + r * k), 1.0 - 2.0 * (i * i + k * k), 2.0 * (j * k - r * i));
				Vector3 result = Vector3(0.0, 1.0, 0.0).rotate(*this);
				//result.normalise();
				return result;
			}

			Vector3 getDown()
			{
				return Vector3(-2.0 * (i * j + r * k), -(1.0 - 2.0 * (i * i + k * k)), -2.0 * (j * k - r * i));
				//return Vector3(0.0, -1.0, 0.0).rotate(*this);
			}

			Vector3 getRight()
			{
				Vector3 result = Vector3(1.0 - 2.0 * (j * j + k * k), 2.0 * (i * j - r * k), 2.0 * (i * k + r * j));
				//result.normalise();
				return result;
				//return Vector3(1.0, 0.0, 0.0).rotate(*this);
			}

			Vector3 getLeft()
			{
				Vector3 result = Vector3(-(1.0 - 2.0 * (j * j + k * k)), -2.0 * (i * j - r * k), -2.0 * (i * k + r * j));
				//result.normalise();
				return result;
				//return Vector3(-1.0, 0.0, 0.0).rotate(*this);
			}

			Quaternion initRotation(Vector3 axis, real angle)
			{
				real sinHalfAngle = std::sin((angle / 2) * R_PI/180);
				real cosHalfAngle = std::cos((angle / 2) * R_PI/180);

				i = axis.x * sinHalfAngle;
				j = axis.y * sinHalfAngle;
				k = axis.z * sinHalfAngle;
				r = cosHalfAngle;

				return Quaternion(r, i, j, k);
			}

			//Matrix4x4 toRotation();
    };

    /**
        What this class is going to hold is information on the transformation matrix which 3 by 3
        This will be used for the rendering of 3D objects.
    */
    class Matrix3
    {
        public:
            //Hold the tensor matrix data in vector form.
            real data[3][3];

            //Constructs a new matrix
            Matrix3()
            {
                data[0][0] = data[0][1] = data[0][2] = 0;
                data[1][0] = data[1][1] = data[1][2] = 0;
                data[2][0] = data[2][1] = data[2][2] = 0;
            }

            //Constructs a new matrix with arguments
            Matrix3(real m0, real m1, real m2, real m3, real m4, real m5, real m6, real m7, real m8)
            {
                data[0][0] = m0; data[0][1] = m1; data[0][2] = m2;
                data[1][0] = m3; data[1][1] = m4; data[1][2] = m5;
                data[2][0] = m6; data[2][1] = m7; data[2][2] = m8;
            }

            //Sets up the matrix using vectors.
            Matrix3(const Vector3 &vec1, const Vector3 &vec2, const Vector3 &vec3)
            {
                setComponents(vec1, vec2, vec3);
            }

            //This function sets up the matrix to hold values in a diagonal matrix.
            //This function uses the setInertiaTensor to set up the matrix for the diagonal matrix.
            void setDiagonal(real a, real b, real c)
            {
                setInertiaTensorCoeff(a, b, c);
            }

            //This function sets the values for the inertia tensor
            void setInertiaTensorCoeff(real ix, real iy, real iz, real ixy = 0, real ixz = 0, real iyz = 0)
            {
                data[0][0] = ix;
                data[0][1] = data[1][0] = -ixy;
                data[0][1] = data[2][0] = -ixz;
                data[1][1] = iy;
                data[1][2] = data[2][1] = -iyz;
                data[2][2] = iz;
            }

            //Sets the values of the matrix to an inertia tensor of a rectangle block aligned, with the body's coordinates system, ie the local basis.
            //Give the axis half size and mass.
            void setBlockInertiaTensor(const Vector3 &halfSize, real mass)
            {
                Vector3 square = halfSize.componentProduct(halfSize);

                setInertiaTensorCoeff(0.3 * mass * (square.y + square.z), 0.3 * mass * (square.x + square.z), 0.3 * mass * (square.x + square.y));
            }

            //Sets the matrix to be a skew symmetric matrix based on the given vector.
            //The skew symmetric matrix is the same as the vector product.
            //So if we have two vectors a x b = c, c is a skewed symmetric form of a.
            //We apply this to our 3 by 3 matrix.
            void setSkewSymmetric(const Vector3 &vec)
            {
                data[0][0] = data[1][1] = data[2][2] = 0;
                data[0][1] = -vec.z;
                data[0][2] = vec.y;
                data[1][0] = vec.z;
                data[1][2] = -vec.x;
                data[2][0] = -vec.y;
                data[2][1] = vec.x;
            }

            //Overloads the multiplication operator, multiplying the 3 by 3 matrix with the core vector.
            Vector3 operator*(const Vector3 &vec) const
            {
                return Vector3(
                       vec.x * data[0][0] + vec.y * data[0][1] + vec.z * data[0][2],
                       vec.x * data[1][0] + vec.y * data[1][1] + vec.z * data[1][2],
                       vec.x * data[2][0] + vec.y * data[2][1] + vec.z * data[2][2]);
            }

            //Multiplies a 3 by 3 matrix with another.
            Matrix3 operator*(const Matrix3 &matrix) const
            {
                return Matrix3(data[0][0] * matrix.data[0][0] + data[0][1] * matrix.data[1][0] + data[0][2] * matrix.data[2][0],
                               data[0][0] * matrix.data[0][1] + data[0][1] * matrix.data[1][1] + data[0][2] * matrix.data[2][1],
                               data[0][0] * matrix.data[0][2] + data[0][1] * matrix.data[1][2] + data[0][2] * matrix.data[2][2],

                               data[1][0] * matrix.data[0][0] + data[1][1] * matrix.data[1][0] + data[1][2] * matrix.data[2][0],
                               data[1][0] * matrix.data[0][1] + data[1][1] * matrix.data[1][1] + data[1][2] * matrix.data[2][1],
                               data[1][0] * matrix.data[0][2] + data[1][1] * matrix.data[1][2] + data[1][2] * matrix.data[2][2],

                               data[2][0] * matrix.data[0][0] + data[2][1] * matrix.data[1][0] + data[2][2] * matrix.data[2][0],
                               data[2][0] * matrix.data[0][1] + data[2][1] * matrix.data[1][1] + data[2][2] * matrix.data[2][1],
                               data[2][0] * matrix.data[0][2] + data[2][1] * matrix.data[1][2] + data[2][2] * matrix.data[2][2]
                               );
            }

            void operator*=(const Matrix3 &matrix)
            {
                real t1, t2, t3;
                t1 = data[0][0] * matrix.data[0][0] + data[0][1] * matrix.data[1][0] + data[0][2] * matrix.data[2][0];
                t2 = data[0][0] * matrix.data[0][1] + data[0][1] * matrix.data[1][1] + data[0][2] * matrix.data[2][1];
                t3 = data[0][0] * matrix.data[0][2] + data[0][1] * matrix.data[1][2] + data[0][2] * matrix.data[2][2];
                data[0][0] = t1;
                data[0][1] = t2;
                data[0][2] = t3;

                t1 = data[1][0] * matrix.data[0][0] + data[1][1] * matrix.data[1][0] + data[1][2] * matrix.data[2][0];
                t2 = data[1][0] * matrix.data[0][1] + data[1][1] * matrix.data[1][1] + data[1][2] * matrix.data[2][1];
                t3 = data[1][0] * matrix.data[0][2] + data[1][1] * matrix.data[1][2] + data[1][2] * matrix.data[2][2],
                data[1][0] = t1;
                data[1][1] = t2;
                data[1][2] = t3;

                t1 = data[2][0] * matrix.data[0][0] + data[2][1] * matrix.data[1][0] + data[2][2] * matrix.data[2][0];
                t2 = data[2][0] * matrix.data[0][1] + data[2][1] * matrix.data[1][1] + data[2][2] * matrix.data[2][1];
                t3 = data[2][0] * matrix.data[0][2] + data[2][1] * matrix.data[1][2] + data[2][2] * matrix.data[2][2];
                data[2][0] = t1;
                data[2][1] = t2;
                data[2][2] = t3;
            }

            //This multiplies a given matrix by a scalar
            void operator*=(const real &scalar)
            {
                data[0][0] *= scalar; data[0][1] *= scalar; data[0][2] *= scalar;
                data[1][0] *= scalar; data[1][1] *= scalar; data[1][2] *= scalar;
                data[2][0] *= scalar; data[2][1] *= scalar; data[2][2] *= scalar;
            }

            //Simply adds the matrix given to the current matrix
            void operator+=(const Matrix3 &matrix)
            {
                data[0][0] += matrix.data[0][0]; data[0][1] += matrix.data[0][1]; data[0][2] += matrix.data[0][2];
                data[1][0] += matrix.data[1][0]; data[1][1] += matrix.data[1][1]; data[1][2] += matrix.data[1][2];
                data[2][0] += matrix.data[2][0]; data[2][1] += matrix.data[2][1]; data[2][2] += matrix.data[2][2];
            }

			//This functions takes any matrix and sets up the idenity but overwrites any other value.
			Matrix3 setIdentity()
			{
                data[0][0] = 1; data[0][1] = 0; data[0][2] = 0;
                data[1][0] = 0; data[1][1] = 1; data[1][2] = 0;
                data[2][0] = 0; data[2][1] = 0; data[2][2] = 1;

				return *this;
			}

			real getDeterminant()
			{
                return(data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]) -
                data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0]) +
                data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]));
			}

            real getDeterminant(real eigen)
			{
                return((data[0][0] - eigen) * ((data[1][1] - eigen) * (data[2][2] - eigen) - data[1][2] * data[2][1]) -
                data[0][1] * (data[1][0] * (data[2][2] - eigen) - data[1][2] * data[2][0]) +
                data[0][2] * (data[1][0] * data[2][1] - (data[1][1] - eigen) * data[2][0]));
			}


            //This functions transforms a matrix by multiplying by a vector.
            Vector3 transform(const Vector3 &vec) const
            {
                return (*this) * vec;
            }

            //This function returns a vector which is transformed by the current matrix.
            Vector3 transformTranspose(const Vector3& vec)
            {
                return Vector3(vec.x * data[0][0] + vec.y * data[1][0] + vec.z * data[2][0],
                               vec.x * data[0][1] + vec.y * data[1][1] + vec.z * data[2][1],
                               vec.x * data[0][2] + vec.y * data[1][2] + vec.z * data[2][2]);
            }

            //This function simply returns a row(which is the column of the current matrix) in a vector
            Vector3 getRowVector(int i)
            {
                return Vector3(data[i][0], data[i + 1][0], data[i + 2][0]);
            }

            //This function simply returns an axis(which is the column of the current matrix) in a vector
            Vector3 getAxisVector(int i)
            {
                return Vector3(data[0][i], data[0][i + 1], data[0][i + 2]);
            }

            /**This function inverts the matrix*/
            void setInverse(const Matrix3 &matrix)
            {
                real t1 = matrix.data[0][0] * matrix.data[1][1];
                real t2 = matrix.data[0][0] * matrix.data[1][2];

                real t3 = matrix.data[0][1] * matrix.data[1][0];
                real t4 = matrix.data[0][2] * matrix.data[1][0];

                real t5 = matrix.data[0][1] * matrix.data[2][0];
                real t6 = matrix.data[0][2] * matrix.data[2][0];

                //Now to calculate the determinant.
                real det = (t1 * matrix.data[2][2] - t2 * matrix.data[2][1] - t3 * matrix.data[2][2] +
                            t4 * matrix.data[2][1] + t5 * matrix.data[1][2] - t6 * matrix.data[1][1]);

                //This is to make sure we do not divide
                if(det == (real)0.0)
                {
                    return;
                }

                real invd = (real)1.0 / det;

                data[0][0] = (matrix.data[1][1] * matrix.data[2][2] - matrix.data[1][2] * matrix.data[2][1]) * invd;
                data[0][1] = -(matrix.data[0][1] * matrix.data[2][2] - matrix.data[0][2] * matrix.data[2][1]) * invd;
                data[0][2] = (matrix.data[0][1] * matrix.data[1][2] - matrix.data[0][2] * matrix.data[1][1]) * invd;

                data[1][0] = -(matrix.data[1][0] * matrix.data[2][2]- matrix.data[1][2] * matrix.data[2][0]) * invd;
                data[1][1] = (matrix.data[0][0] * matrix.data[2][2] - t6) * invd;
                data[1][2] = -(t2 - t4) * invd;

                data[2][0] = (matrix.data[1][0] * matrix.data[2][1] - matrix.data[1][1] * matrix.data[2][0]) * invd;
                data[2][1] = -(matrix.data[0][0] * matrix.data[2][1] - t5) * invd;
                data[2][2] = (t1 - t3) * invd;

            }

            //This function takes the argument of three vector and applies that to the matrix.
            void setComponents(const Vector3 &vec1, const Vector3 &vec2, const Vector3 &vec3)
            {
                data[0][0] = vec1.x; data[0][1] = vec2.x; data[0][2] = vec3.x;
                data[1][0] = vec1.y; data[1][1] = vec2.y; data[1][2] = vec3.y;
                data[2][0] = vec1.z; data[2][1] = vec2.z; data[2][2] = vec3.z;
            }

            //This function returns the inverse of the matrix we are using for transformation.
            Matrix3 inverse() const
            {
                Matrix3 result;
                result.setInverse(*this);
                return result;
            }

            //This function inverts the matrix
            void invert()
            {
                setInverse(*this);
            }

            //This function transposes the currently matrix.
            void setTranspose(const Matrix3 &matrix)
            {
                data[0][0] = matrix.data[0][0]; data[0][1] = matrix.data[1][0]; data[0][2] = matrix.data[2][0];
                data[1][0] = matrix.data[0][1]; data[1][1] = matrix.data[1][1]; data[1][2] = matrix.data[2][1];
                data[2][0] = matrix.data[0][2]; data[2][1] = matrix.data[1][2]; data[2][2] = matrix.data[2][2];
            }

            //This function returns a new matrix which is the transpose of the current matrix.
            Matrix3 transpose() const
            {
                Matrix3 result;
                result.setTranspose(*this);
                return result;
            }

            //This function converts the quaternion into a rotational matrix.
            void setOrientation(const Quaternion &quat)
            {
                data[0][0] = 1 - (2 * quat.j * quat.j + 2 * quat.k * quat.k);
                data[0][1] = 2 * quat.i * quat.j + 2 * quat.k * quat.r;
                data[0][2] = 2 * quat.i * quat.k - 2 * quat.j * quat.r;

                data[1][0] = 2 * quat.i * quat.j - 2 * quat.k * quat.r;
                data[1][1] = 1 - (2 * quat.i * quat.i + 2 * quat.k * quat.k);
                data[1][2] = 2 * quat.j * quat.k + 2 * quat.i * quat.r;

                data[2][0] = 2 * quat.i * quat.k + 2 * quat.j * quat.r;
                data[2][1] = 2 * quat.j * quat.k - 2 * quat.i * quat.r;
                data[2][2] = 1 - (2 * quat.i * quat.i + 2 * quat.j * quat.j);
            }

            //This function interpolates a couple of matrices.
            static Matrix3 linearInterpolate(const Matrix3 &mat1, const Matrix3 &mat2, real prop);

    };

    /**
        Holds the transformation matrix but also considers the rotation and position of a rigid body entity.
        This will be used for rendering 3D object.
    */
    class Matrix4
    {
        public:
            //Holds the transform matrix in vector form.
            real data[3][4];

            Matrix4()
            {
                data[0][1] = data[0][2] = data[0][3] = data[1][0] = data[1][2] = data[1][3] = data[2][0] = data[2][1] = data[2][3] = 0;
                data[0][0] = data[1][1] = data[2][2] = 1;
            }

            //This function sets the matrix to be a diagonal matrix.
            void setDiagonal(real a, real b, real c)
            {
                data[0][0] = a;
                data[1][1] = b;
                data[2][2] = c;
            }

            //Transforms a vector by this matrix
            Vector3 operator*(const Vector3 &vec) const
            {
                return Vector3(vec.x * data[0][0] +
                               vec.y * data[0][1] +
                               vec.z * data[0][2] + data[0][3],

                               vec.x * data[1][0] +
                               vec.y * data[1][1] +
                               vec.z * data[1][2] + data[1][3],

                               vec.x * data[2][0] +
                               vec.y * data[2][1] +
                               vec.z * data[2][2] + data[2][3]);
            }

            Matrix4 operator*(const Matrix4 &matrix) const
            {
                Matrix4 result;
                result.data[0][0] = (matrix.data[0][0] * data[0][0]) + (matrix.data[1][0] * data[0][1]) + (matrix.data[2][0] * data[0][2]);
                result.data[1][0] = (matrix.data[0][0] * data[1][0]) + (matrix.data[1][0] * data[1][1]) + (matrix.data[2][0] * data[1][2]);
                result.data[2][0] = (matrix.data[0][0] * data[2][0]) + (matrix.data[1][0] * data[2][1]) + (matrix.data[2][0] * data[2][2]);

                result.data[0][1] = (matrix.data[0][1] * data[0][0]) + (matrix.data[1][1] * data[0][1]) + (matrix.data[2][1] * data[0][2]);
                result.data[1][1] = (matrix.data[0][1] * data[1][0]) + (matrix.data[1][1] * data[1][1]) + (matrix.data[2][1] * data[1][2]);
                result.data[2][1] = (matrix.data[0][1] * data[2][0]) + (matrix.data[1][1] * data[2][1]) + (matrix.data[2][1] * data[2][2]);

                result.data[0][2] = (matrix.data[0][2] * data[0][0]) + (matrix.data[1][2] * data[0][1]) + (matrix.data[2][2] * data[0][2]);
                result.data[1][2] = (matrix.data[0][2] * data[1][0]) + (matrix.data[1][2] * data[1][1]) + (matrix.data[2][2] * data[1][2]);
                result.data[2][2] = (matrix.data[0][2] * data[2][0]) + (matrix.data[1][2] * data[2][1]) + (matrix.data[2][2] * data[2][2]);

                result.data[0][3] = (matrix.data[0][3] * data[0][0]) + (matrix.data[1][3] * data[0][1]) + (matrix.data[2][3] * data[0][2]) + data[0][3];
                result.data[1][3] = (matrix.data[0][3] * data[1][0]) + (matrix.data[1][3] * data[1][1]) + (matrix.data[2][3] * data[1][2]) + data[1][3];
                result.data[2][3] = (matrix.data[0][3] * data[2][0]) + (matrix.data[1][3] * data[2][1]) + (matrix.data[2][3] * data[2][2]) + data[2][3];

                return result;
            }

            Vector3 transform(const Vector3 &vec) const
            {
                return (*this) * vec;
            }

            //Gets the the determinant for the 4 by 4 matrix.
            real getDeterminant() const;

            //This function inverts the matrix given in the arguments
            void setInverse(const Matrix4 &matrix);

            //Returns a new matrix of he inverse matrix we are currently using.
            Matrix4 inverse()
            {
                Matrix4 result;
                result.setInverse(*this);
                return result;
            }

            //This function simply inverts the matrix
            void invert()
            {
                setInverse(*this);
            }

            //This function set up the rotation matrix and the point of origin.
            void setOrientationAndPos(const Quaternion& quat, const Vector3 &pos)
            {
                data[0][0] = 1 - (2 * quat.j * quat.j + 2 * quat.k * quat.k);
                data[0][1] = 2 * quat.i * quat.j + 2 * quat.k * quat.r;
                data[0][2] = 2 * quat.i * quat.k - 2 * quat.j * quat.r;
                data[0][3] = pos.x;

                data[1][0] = 2 * quat.i * quat.j - 2 * quat.k * quat.r;
                data[1][1] = 1 - (2 * quat.i * quat.i + 2 * quat.k * quat.k);
                data[1][2] = 2 * quat.j * quat.k + 2 * quat.i * quat.r;
                data[1][3] = pos.y;

                data[2][0] = 2 * quat.i * quat.k + 2 * quat.j * quat.r;
                data[2][1] = 2 * quat.j * quat.k - 2 * quat.i * quat.r;
                data[2][2] = 1 - (2 * quat.i * quat.i + 2 * quat.j * quat.j);
                data[2][3] = pos.z;
            }

            //This function given vector by transformational inverse of this matrix.
            Vector3 transformInverse(const Vector3 &vec) const
            {
                Vector3 temp = vec;
                temp.x -= data[0][3];
                temp.y -= data[1][3];
                temp.z -= data[2][3];
                return Vector3(temp.x * data[0][0] +
                               temp.y * data[1][0] +
                               temp.z * data[2][0],

                                temp.x * data[0][1] +
                                temp.y * data[1][1] +
                                temp.z * data[2][1],

                                temp.x * data[0][2] +
                                temp.y * data[1][2] +
                                temp.z * data[2][2]);
            }

            //This functions returns one column of the matrix and returns that to a vector
            Vector3 getAxisVector(int i) const
            {
                Vector3 result;
                for(int j = 0; j < 3; j++)
                {
                    result[j] = data[j][i];

                }

                return result;
            }

            //This function takes the local coordinate to a world coordinate.
            Vector3 localToWorld(const Vector3 &local, const Matrix4 &trans)
            {
                return trans.transform(local);
            }

            //This function takes the world coordinate to a local coordinate.
            Vector3 worldToLocalInverse(const Vector3 &world, const Matrix4 &trans)
            {
                Matrix4 inverseTransform;
                inverseTransform.setInverse(trans);

                return inverseTransform.transform(world);
            }


            Vector3 worldToLocal(const Vector3 &world, const Matrix4 &trans)
            {
                return trans.transformInverse(world);
            }

            //This is a special function which allows transformation for a 3 by 4 matrix to a direction vector.
            Vector3 transformDirection(const Vector3 &vec) const
            {

                return Vector3(vec.x * data[0][0] +
                               vec.y * data[0][1] +
                               vec.z * data[0][2],

                               vec.x * data[1][0] +
                               vec.y * data[1][1] +
                               vec.z * data[1][2],

                               vec.x * data[2][0] +
                               vec.y * data[2][1] +
                               vec.z * data[2][2]);
            }

            //This is a special function which allows transformation for a 3 by 4 matrix to a direction vector and inverses it.
            Vector3 transformInverseDirection(const Vector3 &vec) const
            {
                return Vector3(vec.x * data[0][0] +
                               vec.y * data[1][0] +
                               vec.z * data[2][0],

                               vec.x * data[0][1] +
                               vec.y * data[1][1] +
                               vec.z * data[2][1],

                               vec.x * data[0][2] +
                               vec.y * data[1][2] +
                               vec.z * data[2][2]);
            }

            //This function returns the local coordinate from world coordinate for the directional vector.
            Vector3 localToWorldDir(const Vector3 &local, const Matrix4 &trans)
            {
                return trans.transformDirection(local);
            }

            //This function returns the local coordinate from world coordinate for the directional vector.
            Vector3 worldToLocalDir(const Vector3 &world, const Matrix4 &trans)
            {
                return trans.transformInverseDirection(world);
            }

            //This set our 3 by 4 matrix into a GL array so it can used for rendering.
            void fillGLArray(float GLarray[16]) const
            {
                GLarray[0] = static_cast<float>(data[0][0]);
                GLarray[1] = static_cast<float>(data[1][0]);
                GLarray[2] = static_cast<float>(data[2][0]);
                GLarray[3] = 0.f;

                GLarray[4] = static_cast<float>(data[0][1]);
                GLarray[5] = static_cast<float>(data[1][1]);
                GLarray[6] = static_cast<float>(data[2][1]);
                GLarray[7] = 0.f;

                GLarray[8] = static_cast<float>(data[0][2]);
                GLarray[9] = static_cast<float>(data[1][2]);
                GLarray[10] = static_cast<float>(data[2][2]);
                GLarray[11] = 0.f;

                GLarray[12] = static_cast<float>(data[0][3]);
                GLarray[13] = static_cast<float>(data[1][3]);
                GLarray[14] = static_cast<float>(data[2][3]);
                GLarray[15] = 1.f;
            }
    };

	/**
		This is a limited class that handles 4 by 4 matrices.
	*/
	class Matrix4x4
	{
		public:
			real data[4][4];

			//Constructor sets up the idenity.
			Matrix4x4()
            {
                data[0][1] = data[0][2] = data[0][3] = data[1][0] = data[1][2] = data[1][3] = data[2][0] = data[2][1] = data[2][3] = data[3][0] = data[3][1] = data[3][2] = 0;
                data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1;
            }

			//Sets a everything to a scalar
			Matrix4x4(real scalar)
            {
                for(int i = 0; i < 4; i++)
                {
                    for(int j = 0; j < 4; j++)
                    {
                        data[i][j] = scalar;
                    }
                }
            }

			Matrix4x4(float mat[16])
            {
                data[0][0] = mat[0];
				data[0][1] = mat[4];
				data[0][2] = mat[8];

				data[1][0] = mat[1];
				data[1][1] = mat[5];
				data[1][2] = mat[9];

				data[2][0] = mat[2];
				data[2][1] = mat[6];
				data[2][2] = mat[10];

				data[3][0] = mat[3];
				data[3][1] = mat[7];
				data[3][2] = mat[11];

				data[0][3] = mat[12];
				data[1][3] = mat[13];
				data[2][3] = mat[14];
				data[3][3] = mat[15];
            }

			//This functions takes any matrix and sets up the idenity but overwrites any other value.
			Matrix4x4 setIdentity()
			{
				data[0][0] = 1; data[0][1] = 0; data[0][2] = 0; data[0][3] = 0;
				data[1][0] = 0; data[1][1] = 1; data[1][2] = 0; data[1][3] = 0;
				data[2][0] = 0; data[2][1] = 0; data[2][2] = 1; data[2][3] = 0;
				data[3][0] = 0; data[3][1] = 0; data[3][2] = 0; data[3][3] = 1;

				return *this;
			}

			Matrix4x4 setTranslation(const real& x, const real& y, const real& z)
			{
				data[0][0] = 1; data[0][1] = 0; data[0][2] = 0; data[0][3] = x;
				data[1][0] = 0; data[1][1] = 1; data[1][2] = 0; data[1][3] = y;
				data[2][0] = 0; data[2][1] = 0; data[2][2] = 1; data[2][3] = z;
				data[3][0] = 0; data[3][1] = 0; data[3][2] = 0; data[3][3] = 1;

				return *this;
			}


			Matrix4x4 initRotation(Vector3 forward, Vector3 up)
			{
				Vector3 f = forward;
				f.normalise();

				Vector3 r = up;
				r.normalise();
				r = r % f;

				Vector3 u = f % r;

				return initRotation(f, u, r);
			}

			Matrix4x4 initRotation(Vector3 forward, Vector3 up, Vector3 right)
			{
				Matrix4x4 result;
				Vector3 f = forward;
				Vector3 r = right;
				Vector3 u = up;

				result.data[0][0] = r.x; result.data[0][1] = r.y; result.data[0][2] = r.z; result.data[0][3] = 0;
				result.data[1][0] = u.x; result.data[1][1] = u.y; result.data[1][2] = u.z; result.data[1][3] = 0;
				result.data[2][0] = f.x; result.data[2][1] = f.y; result.data[2][2] = f.z; result.data[2][3] = 0;
				result.data[3][0] = 0; result.data[3][1] = 0; result.data[3][2] = 0; result.data[3][3] = 1;

				return result;
			}

			//This gets the current determinate of a 4 by 4.
			//Note: If this is too slow find a better. Is it faster to built a triangle matrix first?
			real getDeterminant()
			{
                Matrix3 minor1(data[1][1], data[1][2], data[1][3],
                               data[2][1], data[2][2], data[2][3],
                               data[3][1], data[3][2], data[3][3]);

                Matrix3 minor2(data[1][0], data[1][2], data[1][3],
                               data[2][0], data[2][2], data[2][3],
                               data[3][0], data[3][2], data[3][3]);

                Matrix3 minor3(data[1][0], data[1][1], data[1][3],
                               data[2][0], data[2][1], data[2][3],
                               data[3][0], data[3][1], data[3][3]);

                Matrix3 minor4(data[1][0], data[1][1], data[1][2],
                               data[2][0], data[2][1], data[2][2],
                               data[3][0], data[3][1], data[3][2]);

                return ((data[0][0] * minor1.getDeterminant()) -
                        (data[0][1] * minor2.getDeterminant()) +
                        (data[0][2] * minor3.getDeterminant()) -
                        (data[0][3] * minor4.getDeterminant()));
			}

			//Simple multiplication of a 4 by 4 matrix.
			Matrix4x4 operator*(const Matrix4x4& matrix)
			{
				Matrix4x4 result;
                result.data[0][0] = (data[0][0] * matrix.data[0][0]) + (data[0][1] * matrix.data[1][0]) + (data[0][2] * matrix.data[2][0]) + (data[0][3] * matrix.data[3][0]);
                result.data[1][0] = (data[1][0] * matrix.data[0][0]) + (data[1][1] * matrix.data[1][0]) + (data[1][2] * matrix.data[2][0]) + (data[1][3] * matrix.data[3][0]);
                result.data[2][0] = (data[2][0] * matrix.data[0][0]) + (data[2][1] * matrix.data[1][0]) + (data[2][2] * matrix.data[2][0]) + (data[2][3] * matrix.data[3][0]);
				result.data[3][0] = (data[3][0] * matrix.data[0][0]) + (data[3][1] * matrix.data[1][0]) + (data[3][2] * matrix.data[2][0]) + (data[3][3] * matrix.data[3][0]);

                result.data[0][1] = (data[0][0] * matrix.data[0][1]) + (data[0][1] * matrix.data[1][1]) + (data[0][2] * matrix.data[2][1]) + (data[0][3] * matrix.data[3][1]);
                result.data[1][1] = (data[1][0] * matrix.data[0][1]) + (data[1][1] * matrix.data[1][1]) + (data[1][2] * matrix.data[2][1]) + (data[1][3] * matrix.data[3][1]);
                result.data[2][1] = (data[2][0] * matrix.data[0][1]) + (data[2][1] * matrix.data[1][1]) + (data[2][2] * matrix.data[2][1]) + (data[2][3] * matrix.data[3][1]);
				result.data[3][1] = (data[3][0] * matrix.data[0][1]) + (data[3][1] * matrix.data[1][1]) + (data[3][2] * matrix.data[2][1]) + (data[3][3] * matrix.data[3][1]);

                result.data[0][2] = (data[0][0] * matrix.data[0][2]) + (data[0][1] * matrix.data[1][2]) + (data[0][2] * matrix.data[2][2]) + (data[0][3] * matrix.data[3][2]);
                result.data[1][2] = (data[1][0] * matrix.data[0][2]) + (data[1][1] * matrix.data[1][2]) + (data[1][2] * matrix.data[2][2]) + (data[1][3] * matrix.data[3][2]);
                result.data[2][2] = (data[2][0] * matrix.data[0][2]) + (data[2][1] * matrix.data[1][2]) + (data[2][2] * matrix.data[2][2]) + (data[2][3] * matrix.data[3][2]);
				result.data[3][2] = (data[3][0] * matrix.data[0][2]) + (data[3][1] * matrix.data[1][2]) + (data[3][2] * matrix.data[2][2]) + (data[3][3] * matrix.data[3][2]);

                result.data[0][3] = (data[0][0] * matrix.data[0][3]) + (data[0][1] * matrix.data[1][3]) + (data[0][2] * matrix.data[2][3]) + (data[0][3] * matrix.data[3][3]);
                result.data[1][3] = (data[1][0] * matrix.data[0][3]) + (data[1][1] * matrix.data[1][3]) + (data[1][2] * matrix.data[2][3]) + (data[1][3] * matrix.data[3][3]);
                result.data[2][3] = (data[2][0] * matrix.data[0][3]) + (data[2][1] * matrix.data[1][3]) + (data[2][2] * matrix.data[2][3]) + (data[2][3] * matrix.data[3][3]);
				result.data[3][3] = (data[3][0] * matrix.data[0][3]) + (data[3][1] * matrix.data[1][3]) + (data[3][2] * matrix.data[2][3]) + (data[3][3] * matrix.data[3][3]);

				return result;
			}

			void getGLTransform(float GLarray[16]) const
			{
				GLarray[0] = static_cast<float>(data[0][0]);
				GLarray[1] = static_cast<float>(data[1][0]);
				GLarray[2] = static_cast<float>(data[2][0]);
				GLarray[3] = static_cast<float>(data[3][0]);

				GLarray[4] = static_cast<float>(data[0][1]);
				GLarray[5] = static_cast<float>(data[1][1]);
				GLarray[6] = static_cast<float>(data[2][1]);
				GLarray[7] = static_cast<float>(data[3][1]);

				GLarray[8] = static_cast<float>(data[0][2]);
				GLarray[9] = static_cast<float>(data[1][2]);
				GLarray[10] = static_cast<float>(data[2][2]);
				GLarray[11] = static_cast<float>(data[3][2]);

				GLarray[12] = static_cast<float>(data[0][3]);
				GLarray[13] = static_cast<float>(data[1][3]);
				GLarray[14] = static_cast<float>(data[2][3]);
				GLarray[15] = static_cast<float>(data[3][3]);
			}

			Matrix4x4 perspectiveRH(real fovy, real aspect, real zNear, real zFar)
			{
				if(!(std::abs(aspect - std::numeric_limits<real>::epsilon()) > 0.0))
                {
                    std::cerr << "Error aspect minus epsilon is bigger than 0" << std::endl;
                    std::cerr << (std::abs(aspect - std::numeric_limits<real>::epsilon())) << std::endl;
                }

				real const tanHalfFovy = tan(fovy / 2.0);

				wind::Matrix4x4 Result(0.0);
				Result.data[0][0] = 1.0 / (aspect * tanHalfFovy);
				Result.data[1][1] = 1.0 / (tanHalfFovy);
				Result.data[3][2] = -1.0;

				//Only works if the clip depth is not 0!
				Result.data[2][2] = -(zFar + zNear) / (zFar - zNear);
				Result.data[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);

				return Result;
			}

			/**
				This function is for handling 2D projection on. Needed for text projections and other GUIs
				Note: Only works if clip depth is zero and it's in RH coordinates.
			*/
			Matrix4x4 orthoRH(real left, real right, real bottom, real top, real zNear, real zFar)
			{
				Matrix4x4 Result;
				Result.data[0][0] = 2.0 / (right - left);
				Result.data[1][1] = 2.0 / (top - bottom);
				Result.data[0][3] = -(right + left) / (right - left);
				Result.data[1][3] = -(top + bottom) / (top - bottom);

				//Commented out if clip depth is zero solution.
				if(zNear == 0 && zFar == 0)
				{
					Result.data[2][2] = -1.0 / (zFar - zNear);
					Result.data[3][2] = -zNear / (zFar - zNear);
					Result.data[3][3] = 1;
				}
				else
				{
					//Only works if clip depth is not equal to zero.
					Result.data[2][2] = -2.0 / (zFar - zNear);
					Result.data[2][3] = -(zFar + zNear) / (zFar - zNear);
					Result.data[3][3] = 1;
				}

				return Result;
			}

			Matrix4x4 lookAt(Vector3 eye, Vector3 centre, Vector3 up)
			{
				Vector3 f(centre - eye);
				f.normalise();

				Vector3 s(f % up);
				s.normalise();

				Vector3 u(s % f);

				Matrix4x4 Result(1.0);
				Result.data[0][0] = s.x;
				Result.data[1][0] = s.y;
				Result.data[2][0] = s.z;

				Result.data[0][1] = u.x;
				Result.data[1][1] = u.y;
				Result.data[2][1] = u.z;

				Result.data[0][2] = -f.x;
				Result.data[1][2] = -f.y;
				Result.data[2][2] = -f.z;

				Result.data[3][0] = -s.scalarProduct(eye);
				Result.data[3][1] = -u.scalarProduct(eye);
				Result.data[3][2] = f.scalarProduct(eye);

				return Result;
			}
	};
};
#endif // CORE_H
