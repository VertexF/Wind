#include "Body.h"

using namespace wind;

//This is a inline function the calculates the transformation matrix from the orientation and position.
static inline void calculateTransformMatrix(Matrix4 &transformMatrix, const Vector3 &position, const Quaternion &orientation)
{
    transformMatrix.data[0][0] = 1 - 2 * orientation.j * orientation.j - 2 * orientation.k * orientation.k;
    transformMatrix.data[0][1] = 2 * orientation.i * orientation.j - 2 * orientation.r * orientation.k;
    transformMatrix.data[0][2] = 2 * orientation.i * orientation.k + 2 * orientation.r * orientation.j;
    transformMatrix.data[0][3] = position.x;

    transformMatrix.data[1][0] = 2 * orientation.i * orientation.j + 2 * orientation.r * orientation.k;
    transformMatrix.data[1][1] = 1 - 2 * orientation.i * orientation.i - 2 * orientation.k * orientation.k;
    transformMatrix.data[1][2] = 2 * orientation.j * orientation.k - 2 * orientation.r * orientation.i;
    transformMatrix.data[1][3] = position.y;

    transformMatrix.data[2][0] = 2 * orientation.i * orientation.k - 2 * orientation.r * orientation.j;
    transformMatrix.data[2][1] = 2 * orientation.j * orientation.k + 2 * orientation.r * orientation.i;
    transformMatrix.data[2][2] = 1 - 2 * orientation.i * orientation.i - 2 * orientation.j * orientation.j;
    transformMatrix.data[2][3] = position.z;
}

/**
    This function turns a location basis to a world basis.
    This is done by intertia tensor transform by a quaternion.
*/
static inline void transformInertiaTensor(Matrix3 &ittWorld, const Quaternion &quat, const Matrix3 &ittBody, const Matrix4 &rotmat)
{
    real t1 = rotmat.data[0][0] * ittBody.data[0][0] + rotmat.data[0][1] * ittBody.data[1][0] + rotmat.data[0][2] * ittBody.data[2][0];
    real t2 = rotmat.data[0][0] * ittBody.data[0][1] + rotmat.data[0][1] * ittBody.data[1][1] + rotmat.data[0][2] * ittBody.data[2][1];
    real t3 = rotmat.data[0][0] * ittBody.data[0][2] + rotmat.data[0][1] * ittBody.data[1][2] + rotmat.data[0][2] * ittBody.data[2][2];

    real t4 = rotmat.data[1][0] * ittBody.data[0][0] + rotmat.data[1][1] * ittBody.data[1][0] + rotmat.data[1][2] * ittBody.data[2][0];
    real t5 = rotmat.data[1][0] * ittBody.data[0][1] + rotmat.data[1][1] * ittBody.data[1][1] + rotmat.data[1][2] * ittBody.data[2][1];
    real t6 = rotmat.data[1][1] * ittBody.data[0][2] + rotmat.data[1][1] * ittBody.data[1][2] + rotmat.data[1][2] * ittBody.data[2][2];

    real t7 = rotmat.data[2][0] * ittBody.data[0][0] + rotmat.data[2][1] * ittBody.data[1][0] + rotmat.data[2][2] * ittBody.data[2][0];
    real t8 = rotmat.data[2][0] * ittBody.data[0][1] + rotmat.data[2][1] * ittBody.data[1][1] + rotmat.data[2][2] * ittBody.data[2][1];
    real t9 = rotmat.data[2][0] * ittBody.data[0][2] + rotmat.data[2][1] * ittBody.data[1][2] + rotmat.data[2][2] * ittBody.data[2][2];

    ittWorld.data[0][0] = t1 * rotmat.data[0][0] + t2 * rotmat.data[0][1] + t3 * rotmat.data[0][2];
    ittWorld.data[0][1] = t1 * rotmat.data[1][0] + t2 * rotmat.data[1][1] + t3 * rotmat.data[1][2];
    ittWorld.data[0][2] = t1 * rotmat.data[2][0] + t2 * rotmat.data[2][1] + t3 * rotmat.data[2][2];

    ittWorld.data[1][0] = t4 * rotmat.data[0][0] + t5 * rotmat.data[0][1] + t6 * rotmat.data[0][2];
    ittWorld.data[1][1] = t4 * rotmat.data[1][0] + t5 * rotmat.data[1][1] + t6 * rotmat.data[1][2];
    ittWorld.data[1][2] = t4 * rotmat.data[2][0] + t5 * rotmat.data[2][1] + t6 * rotmat.data[2][2];

    ittWorld.data[2][0] = t7 * rotmat.data[0][0] + t8 * rotmat.data[0][1] + t9 * rotmat.data[0][2];
    ittWorld.data[2][1] = t7 * rotmat.data[1][0] + t8 * rotmat.data[1][1] + t9 * rotmat.data[1][2];
    ittWorld.data[2][2] = t7 * rotmat.data[2][0] + t8 * rotmat.data[2][1] + t9 * rotmat.data[2][2];
}

//This function is similar to the integration particle function, this function will be called every single frame.
void RigidBody::calculateDerivedData()
{
    orientation.normalise();

    //Calculates the transform matrix.
    calculateTransformMatrix(transformMatrix, position, orientation);
	//transformMatrix.setOrientationAndPos(orientation, position);

    //Take the inertia tensor and calculates the inertia tensor to world space.
    transformInertiaTensor(inverseInertiaTensorWorld, orientation, inverseInertiaTensor, transformMatrix);
}

void RigidBody::integrate(real duration)
{
	if(!isAwake)
	{
		return;
	}

    //The first thing we do is work the acceleration.
    lastFrameAcceleration = acceleration;
    lastFrameAcceleration.addScaledVector(forceAccum, inverseMass);

    //Next we work out the torque acceleration from the torque, in world space.
    Vector3 angularAcceleration = inverseInertiaTensorWorld.transform(torqueAccum);

    //Here we update the linear velocity from acceleration and impulse.
    velocity.addScaledVector(lastFrameAcceleration, duration);

    //Here we update the angular velocity from acceleration and impulse.
    rotation.addScaledVector(angularAcceleration, duration);

    //Next we calculate the drag force.
    velocity *= pow(linearDamping, duration);
    rotation *= pow(angularDamping, duration);

    //Here we update the linear position.
    position.addScaledVector(velocity, duration);

    //Here we update the angular position.
    orientation.addScaledVector(rotation, duration);

    //Here we normalise the orientation and update the matrix with a new position and orientation
    calculateDerivedData();

    //Finally we clear everything up.
	clearAccumulator();

	if(canSleep)
	{
		real currentMotion = velocity.scalarProduct(velocity) + rotation.scalarProduct(rotation);

		real bias = pow(0.5, duration);
		motion = bias * motion + (1 - bias) * currentMotion;

		if(motion < EpsilonValue::Epsilon())
		{
			setAwake(false);
		}
		else if(motion > EpsilonValue::Epsilon() * 10)
		{
			motion = EpsilonValue::Epsilon() * 10;
		}
	}
}

void RigidBody::setMass(const real mass)
{
    assert(mass != 0);

    inverseMass = 1.f / mass;
}

real RigidBody::getMass() const
{
    if(inverseMass == 0)
    {
        return std::numeric_limits<real>::max();
    }
    else
    {
        return 1.f / inverseMass;
    }
}

void RigidBody::setInverseMass(const real inverseMass)
{
    RigidBody::inverseMass = inverseMass;
}

real RigidBody::getInverseMass()
{
    return 1.f / inverseMass;
}

bool RigidBody::hasInfiniteMass() const
{
    return (inverseMass <= 0.f);
}

void RigidBody::setInertiaTensor(const Matrix3 &inertiaTensor)
{
    inverseInertiaTensor.setInverse(inertiaTensor);
}

void RigidBody::getInertiaTensor(Matrix3 *inertiaTensor) const
{
    inertiaTensor->setInverse(inverseInertiaTensor);
}

Matrix3 RigidBody::getInertiaTensor() const
{
    Matrix3 it;
    getInertiaTensor(&it);
    return it;
}

void RigidBody::getInertiaTensorWorld(Matrix3 *inertiaTensor) const
{
    inertiaTensor->setInverse(inverseInertiaTensorWorld);
}

Matrix3 RigidBody::getInertiaTensorWorld() const
{
    Matrix3 result;
    getInertiaTensorWorld(&result);
    return result;
}

void RigidBody::setInverseInertiaTensor(const Matrix3 &inverseInertiaTensor)
{
    RigidBody::inverseInertiaTensor = inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensor(Matrix3* inverseInertiaTensor) const
{
    *inverseInertiaTensor = RigidBody::inverseInertiaTensor;
}

Matrix3 RigidBody::getInverseInertiaTensor() const
{
    return inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensorWorld(Matrix3* inverseInertiaTensor) const
{
    *inverseInertiaTensor = RigidBody::inverseInertiaTensorWorld;
}

Matrix3 RigidBody::getInverseInertiaTensorWorld() const
{
    return inverseInertiaTensorWorld;
}

void RigidBody::setDamping(const real linearDamp, const real angularDamp)
{
    linearDamping = linearDamp;
    angularDamping = angularDamp;
}

void RigidBody::setLinearDamping(const real linearDamp)
{
    linearDamping = linearDamp;
}

real RigidBody::getLinearDamping() const
{
    return linearDamping;
}

void RigidBody::setAngularDamping(const real angularDamp)
{
    angularDamping = angularDamp;
}

real RigidBody::getAngularDamping() const
{
    return angularDamping;
}

void RigidBody::setPosition(const Vector3 &pos)
{
    position = pos;
}

void RigidBody::setPosition(const real x, const real y, const real z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

Vector3 RigidBody::getPosition() const
{
    return position;
}

void RigidBody::getPosition(Vector3 *pos) const
{
    *pos = position;
}

void RigidBody::setVelocity(const Vector3 &vel)
{
    velocity = vel;
}

void RigidBody::setVelocity(const real x, const real y, const real z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
}

void RigidBody::addVelocity(const Vector3 &vec)
{
    velocity += vec;
}

Vector3 RigidBody::getVelocity() const
{
    return velocity;
}

void RigidBody::getVelocity(Vector3 *vel) const
{
    *vel = velocity;
}

void RigidBody::setAcceleration(const Vector3 &accel)
{
    acceleration = accel;
}

void RigidBody::setAcceleration(const real x, const real y, const real z)
{
    acceleration.x = x;
    acceleration.y = y;
    acceleration.z = z;
}

Vector3 RigidBody::getAcceleration() const
{
    return acceleration;
}

void RigidBody::getAcceleration(Vector3 *accel) const
{
    *accel = acceleration;
}

Vector3 RigidBody::getLastFrameAcceleration() const
{
    return lastFrameAcceleration;
}

void RigidBody::getLastFrameAcceleration(Vector3 *lastFrameAcc) const
{
    *lastFrameAcc = lastFrameAcceleration;
}

void RigidBody::addForce(const Vector3 &force)
{
    forceAccum += force;
    isAwake = true;
}

void RigidBody::setRotation(const real x, const real y, const real z)
{
    rotation.x = x;
    rotation.y = y;
    rotation.z = z;
}

void RigidBody::setRotation(const Vector3 &rot)
{
    rotation = rot;
}

void RigidBody::addRotation(const Vector3 &rot)
{
    rotation += rot;
}

void RigidBody::getRotation(Vector3 *rot)
{
    *rot = rotation;
}

Vector3 RigidBody::getRotation()
{
    return rotation;
}

void RigidBody::setOrientation(const Quaternion &Orient)
{
    orientation = Orient;
    orientation.normalise();
}

void RigidBody::setOrientation(const real r, const real i, const real j, const real k)
{
    orientation.r = r;
    orientation.i = i;
    orientation.j = j;
    orientation.k = k;
    orientation.normalise();
}

Quaternion RigidBody::getOrientation() const
{
    return orientation;
}

void RigidBody::getOrientation(Quaternion *quat) const
{
    *quat = orientation;
}

void RigidBody::getOrientation(real Matrix[3][3]) const
{
    Matrix[0][0] = transformMatrix.data[0][0];
    Matrix[1][0] = transformMatrix.data[1][0];
    Matrix[2][0] = transformMatrix.data[2][0];

    Matrix[0][1] = transformMatrix.data[0][1];
    Matrix[1][1] = transformMatrix.data[1][1];
    Matrix[2][1] = transformMatrix.data[2][1];

    Matrix[0][2] = transformMatrix.data[0][2];
    Matrix[1][2] = transformMatrix.data[1][2];
    Matrix[2][2] = transformMatrix.data[2][2];
}

void RigidBody::getOrientation(Matrix3 *matrix) const
{
    getOrientation(matrix->data);
}

/**
    Note: memcpy does not work with mathematical matrix representation.
**/

void RigidBody::getTransform(Matrix4 *trans) const
{
    //memcpy(trans, &transformMatrix.data, sizeof(Matrix4));
}

void RigidBody::getTransform(real matrix[16]) const
{
    //memcpy(matrix, transformMatrix.data, sizeof(real) * 12);
    //matrix[12] = matrix[13] = matrix[14] = 0;
    //matrix[15] = 1;
}

void RigidBody::getGLTransform(float matrix[16]) const
{
    matrix[0] = static_cast<float>(transformMatrix.data[0][0]);
    matrix[1] = static_cast<float>(transformMatrix.data[1][0]);
    matrix[2] = static_cast<float>(transformMatrix.data[2][0]);
    matrix[3] = 0.f;

    matrix[4] = static_cast<float>(transformMatrix.data[0][1]);
    matrix[5] = static_cast<float>(transformMatrix.data[1][1]);
    matrix[6] = static_cast<float>(transformMatrix.data[2][1]);
    matrix[7] = 0.f;

    matrix[8] = static_cast<float>(transformMatrix.data[0][2]);
    matrix[9] = static_cast<float>(transformMatrix.data[1][2]);
    matrix[10] = static_cast<float>(transformMatrix.data[2][2]);
    matrix[11] = 0.f;

    matrix[12] = static_cast<float>(transformMatrix.data[0][3]);
    matrix[13] = static_cast<float>(transformMatrix.data[1][3]);
    matrix[14] = static_cast<float>(transformMatrix.data[2][3]);
    matrix[15] = 1.f;
}

Matrix4 RigidBody::getTransform() const
{
    return transformMatrix;
}

Vector3 RigidBody::getPointInLocal(const Vector3 &point) const
{
    return transformMatrix.transformInverse(point);
}

Vector3 RigidBody::getPointInWorld(const Vector3 &point) const
{
    return transformMatrix.transform(point);
}

Vector3 RigidBody::getDirectionInLocalSpace(const Vector3 &direction) const
{
    return transformMatrix.transformInverseDirection(direction);
}

Vector3 RigidBody::getDirectionInWorldSpace(const Vector3 &direction) const
{
    return transformMatrix.transformDirection(direction);
}

void RigidBody::getAccumlatedAcceration(Vector3 *totalAccel) const
{
    *totalAccel = lastFrameAcceleration;
}

Vector3 RigidBody::getAccumlatedAcceration()
{
    return lastFrameAcceleration;
}

void RigidBody::clearAccumulator()
{
    forceAccum.Clear();
    torqueAccum.Clear();
}

void RigidBody::addForceAtBodyPoint(const Vector3 &force, const Vector3 &point)
{
    Vector3 pt = getPointInWorld(point);
    addForceAtPoint(force, pt);
}

void RigidBody::setAwake(const bool awake)
{
	if(awake)
	{
		isAwake = true;
		motion = EpsilonValue::Epsilon() * 2.0;
	}
	else
	{
		isAwake = false;
		velocity.Clear();
		rotation.Clear();
	}
}

void RigidBody::setCanSleep(const bool canSleep)
{
	RigidBody::canSleep = canSleep;

	if(!canSleep && !isAwake)
	{
		setAwake(true);
	}
}

void RigidBody::addForceAtPoint(const Vector3 &force, const Vector3 &point)
{
    Vector3 pt = point;
    pt -= position;

    forceAccum += force;
    torqueAccum += pt % force;

    isAwake = true;
}

void RigidBody::addTorque(const Vector3 &torque)
{
    torqueAccum += torque;
    isAwake = true;
}
