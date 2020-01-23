#ifndef BODY_H_INCLUDED
#define BODY_H_INCLUDED
#include <string.h>
#include <assert.h>
#include <iostream>
#include <limits>

#include "Core.h"

/**
    This class will simulate the core of the physics, the rigid bodies.
*/
namespace wind
{
class RigidBody
{
public:
    RigidBody(){}
    //This function calculates the internal data when the state of the rigid body changes.
    //This needs to be called when we integrate the movement of the rigid body
    void calculateDerivedData();

    //Integrates all the motion and forces to the rigid body.
    void integrate(real duration);

    /** Mass functions */
    //Adds mass to a rigid body
    void setMass(const real mass);

    //get mass of a rigid body
    real getMass() const;

    //Sets the inverse mass
    void setInverseMass(const real inverseMass);

    bool hasInfiniteMass() const;

    //returns the inverse mass.
    real getInverseMass();

    /** Inertia tensor functions */
    void setInertiaTensor(const Matrix3 &inertiaTensor);

    void getInertiaTensor(Matrix3 *inertiaTensor) const;

    Matrix3 getInertiaTensor() const;

    void getInertiaTensorWorld(Matrix3 *inertiaTensor) const;

    Matrix3 getInertiaTensorWorld() const;

    void setInverseInertiaTensor(const Matrix3 &inverseInertiaTensor);

    void getInverseInertiaTensor(Matrix3* inverseInertiaTensor) const;

    Matrix3 getInverseInertiaTensor() const;

    void getInverseInertiaTensorWorld(Matrix3* inverseInertiaTensor) const;

    Matrix3 getInverseInertiaTensorWorld() const;

    /** The Damping functions */
    void setDamping(const real linearDamp, const real angularDamp);

    void setLinearDamping(const real linearDamp);
    real getLinearDamping() const;

    void setAngularDamping(const real angularDamp);
    real getAngularDamping() const;

    /** Linear movement vectors functions */
    //Sets position
    void setPosition(const Vector3 &pos);

    //Sets position
    void setPosition(const real x, const real y, const real z);

    //Get the position vector
    Vector3 getPosition() const;

    //Get the position vector
    void getPosition(Vector3 *pos) const;

    //Sets velocity
    void setVelocity(const Vector3 &vel);

    //Sets velocity
    void setVelocity(const real x, const real y, const real z);

    //This function adds a velocity to a rigid body.
    void addVelocity(const Vector3 &vec);

    //Get the velocity vector
    Vector3 getVelocity() const;

    //Get the velocity vector
    void getVelocity(Vector3 *vel) const;

    //Sets acceleration
    void setAcceleration(const Vector3 &accel);

    //Sets acceleration
    void setAcceleration(const real x, const real y, const real z);

    //Get the acceleration vector
    Vector3 getAcceleration() const;

    //Get the acceleration vector
    void getAcceleration(Vector3 *accel) const;

    //This function gets the lastframeacceration vector.
    Vector3 getLastFrameAcceleration() const;

    //This function gets the lastframeacceration vector.
    void getLastFrameAcceleration(Vector3 *lastFrameAcc) const;

    //This function adds the torque forces together
    void addForce(const Vector3 &force);

    /** Transformation and rotation functions */
    //Sets the rotational vector.
    void setRotation(const real x, const real y, const real z);

    //Sets the rotational vector.
    void setRotation(const Vector3 &rot);

    //Sets the rotational vector.
    void addRotation(const Vector3 &rot);

    //Get the rotational vector.
    void getRotation(Vector3 *rot);

    //Get the rotational vector.
    Vector3 getRotation();

    //Set up the quaternion and normalises the quaternion.
    void setOrientation(const Quaternion &Orient);

    //Set up the quaternion and normalises the quaternion.
    void setOrientation(const real r, const real i, const real j, const real k);

    //Gets the quaternion for orientation.
    Quaternion getOrientation() const;

    //Gets the quaternion for orientation.
    void getOrientation(Quaternion *quat) const;

    //Gets the quaternion passed in by a matrix
    void getOrientation(real Matrix[3][3]) const;

    //Gets the quaternion of the matrix based in.
    void getOrientation(Matrix3 *matrix) const;

    //Small getter and setter functions for awake and sleeping bodies.
    bool getAwake() const
    {
        return isAwake;
    }

    void setAwake(const bool awake = true);

    //This functions workout if a body can sleep or not.
    void setCanSleep(const bool canSleep);

    /** Transform functions */

    //This function will fill the matrix in the argument with the transformation representation of the rigid body orientation.
    void getTransform(Matrix4 *trans) const;

    //This function will fill the matrix array with the transformation representation of the rigid body orientation.
    void getTransform(real matrix[16]) const;

    //This function does the same as the other functions but it makes it suitable for OpenGL to use.
    void getGLTransform(float matrix[16]) const;

    //This is simply a getter function for the matrix4 transformation
    Matrix4 getTransform() const;

    /** World to local and Local to world */
    //Converts a point from world to local.
    Vector3 getPointInLocal(const Vector3 &point) const;

    //Converts a point from local to world.
    Vector3 getPointInWorld(const Vector3 &point) const;

    //This function converts the direction in world space to local space.
    Vector3 getDirectionInLocalSpace(const Vector3 &direction) const;

    //This function converts the direction in local space to world space.
    Vector3 getDirectionInWorldSpace(const Vector3 &direction) const;

    //This function returns the total accumulated force that has been accumulated through one frame.
    //The force being returned is acceleration.
    void getAccumlatedAcceration(Vector3 *totalAccel) const;

    //This function returns the total accumulated force that has been accumulated through one frame.
    //The force being returned is acceleration.
    Vector3 getAccumlatedAcceration();

    //Frame by frame clearing of the accumulated forces.
    void clearAccumulator();

    //This function adds a force to a single point of body.
    //The force and the application  point are given in world coordinate.
    //Because the force is not applied at the centre of mass, it may be split into both force and torque
    void addForceAtPoint(const Vector3 &force, const Vector3 &point);

    //This function adds a force to a single point of body.
    //The force and the application  point are given in world coordinate.
    //This force function is better for applying the force such as spring force or other forces
    void addForceAtBodyPoint(const Vector3 &force, const Vector3 &point);

    //This function add the torque to world coordinate
    void addTorque(const Vector3 &torque);
protected:
    //Holds the inverse mas this allows us to make rigid body moveable or immovable.
    real inverseMass;

    //This holds the damping to linear motion. Needed to take away movement and keep things stable.
    real linearDamping;

    //This holds the damping to angular motion. Needed to take away movement and keep things stable.
    real angularDamping;

    //This holds the motion of a body which allows use to handle putting bodies to sleep.
    real motion;

    //This hold the angular orientation of the rigid body in world space.
    Quaternion orientation;

    //This holds the linear position of the rigid body
    Vector3 position;

    //This holds the linear velocity of the rigid body.
    Vector3 velocity;

    //Holds the acceleration of the rigid body.
    Vector3 acceleration;

    //This holds the angular velocity of the rigid body.
    Vector3 rotation;

    //This vector keeps track of all the added non-torque force together.
    Vector3 forceAccum;

    //This vector keeps track of all the added torque force together.
    Vector3 torqueAccum;

    //Holds the linear acceleration of the rigid body, for the previous frame.
    Vector3 lastFrameAcceleration;

    //This holds the translation from world to local and local to world basis.
    Matrix4 transformMatrix;

    //This holds the inverse of the inertia tensor so we can directly calculate the angular acceleration.
    Matrix3 inverseInertiaTensor;

    //This holds the inverse of the inertia tensor so we can directly calculate the angular acceleration in world space.
    Matrix3 inverseInertiaTensorWorld;

    bool isAwake;

    bool canSleep;
};
}

#endif // BODY_H_INCLUDED
