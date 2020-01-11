#include "contact.h"

using namespace wind;

void Contact::setContactData(RigidBody* bodyOne, RigidBody* bodyTwo, real friction, real restitution)
{
    body[0] = bodyOne;
    body[1] = bodyTwo;
    Contact::friction = friction;
    Contact::restitution = restitution;
}

void Contact::matchAwakeState()
{
    //Collisions with the world never cause a body to wake up.
    if(!body[1])
    {
        return;
    }

    bool body0awake = body[0]->getAwake();
    bool body1awake = body[1]->getAwake();

    //Wake up only the sleeping one
    if(body0awake ^ body1awake)
    {
		if(body0awake)
		{
			body[1]->setAwake(1);
		}
		else
		{
			body[0]->setAwake(1);
		}
    }
}

void Contact::calculateContactBasis()
{
    //These are tangent contact to the contact normal.
    Vector3 contactTangent[2];

    //To get the best orthonormal we need to check if the Z is nearer to the X or the Y axis
    if(std::abs(contactNormal.x) > std::abs(contactNormal.y))
    {
        //Here we make sure everything is normalised.
        const real s = (1.0) / std::sqrt(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);

        //Here we create the orthonormal from the contact point.
        //Here we get the right X is a right angle from the Y.
        contactTangent[0].x = contactNormal.z * s;
        contactTangent[0].y = 0;
        contactTangent[0].z = -contactNormal.x * s;

        contactTangent[1].x = contactNormal.y * contactTangent[0].x;
        contactTangent[1].y = contactNormal.z * contactTangent[0].x - contactNormal.x * contactTangent[0].z;
        contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
    }
    //Else we base the contact normal of the x axis.
    else
    {
        const real s = (1.0) / std::sqrt(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);

        contactTangent[0].x = 0;
        contactTangent[0].y = -contactNormal.z * s;
        contactTangent[0].z = contactNormal.y * s;

        contactTangent[1].x = contactNormal.y * contactTangent[0].z - contactNormal.z * contactTangent[0].y;
        contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
        contactTangent[1].z = contactNormal.x * contactTangent[0].y;
    }

    //This holds all three orthonormal vectors and sets them into a matrix 3 by 3 to make the world contact point.
    contactToWorld.setComponents(contactNormal, contactTangent[0], contactTangent[1]);
}

inline Vector3 Contact::calculateFrictionlessImpulse(Matrix3* inverseInertiaTensor)
{
	//This is here to keep track of the impulse contact force.
	//This is what we will be returning.
	Vector3 impulseContact;

	//Next we need to build a vector that shows the change in velocity in world space for each unit impulse in the direction of the contact normal.
    Vector3 deltaVelWorld = relativeContactPosition[0] % contactNormal;
	deltaVelWorld = inverseInertiaTensor[0].transform(deltaVelWorld);
	deltaVelWorld = deltaVelWorld % relativeContactPosition[0];

	//Here use the dot product to get the scalar value to get the contact cooridate.
	real deltaVelocity = deltaVelWorld * contactNormal;

	//Here we do the simple part, adding the linear motion to the delta velocity.
	deltaVelocity += body[0]->getInverseMass();

	//We run the same code for the second contact
	if(body[1])
	{
		//Next we need to build a vector that shows the change in velocity in world space for each unit impulse in the direction of the contact normal.
		Vector3 deltaVelWorld = relativeContactPosition[1] % contactNormal;
		deltaVelWorld = inverseInertiaTensor[1].transform(deltaVelWorld);
		deltaVelWorld = deltaVelWorld % relativeContactPosition[1];

		//Here use the dot product to get the scalar value to get the contact cooridate.
		deltaVelocity = deltaVelWorld * contactNormal;

		//Here we do the simple part, adding the linear motion to the delta velocity.
		deltaVelocity += body[1]->getInverseMass();
	}

    // Calculate the required size of the impulse
    impulseContact.x = (desiredChangedVelocity / deltaVelocity);
    impulseContact.y = 0;
    impulseContact.z = 0;
    return impulseContact;
}

void Contact::calculateInternals(real duration)
{
    if(!body[0])
    {
        swapBodies();
    }
    assert(body[0]);

    //Here we work out which set of axes the contact point is on.
    calculateContactBasis();

    //Next we store the relative position of the contact relative to each body.
    relativeContactPosition[0] = contactPoint - body[0]->getPosition();

    //Here we do it for body 2 if there is one
    if(body[1])
    {
        relativeContactPosition[1] = contactPoint - body[1]->getPosition();
    }

    //Here we find out the relative velocity of the bodies at the contact point.
    contactVelocity = calculateLocalVelocity(0, duration);
    if(body[1])
    {
        contactVelocity -= calculateLocalVelocity(1, duration);
    }

    calculateDesiredDeltaVelocity(duration);
}

void Contact::swapBodies()
{
    //First we inverse the contact normal.
     contactNormal *= -1;

     RigidBody *temp = body[0];
     body[0] = body[1];
     body[1] = temp;
}

void Contact::makeAwake()
{

}

//void Contact::applyImpulse(const Vector3 &impulse, RigidBody *body, Vector3 *velocityChange, Vector3 *rotationChange)
//{

//}

void Contact::applyVelocityChange(Vector3 velocityChange[2], Vector3 rotationChange[2])
{
	//As with anything to do with rotation and linear motion we need to get the inverse mass and inverse interia tensor
	Matrix3 inverseInertiaTensor[2];
	body[0]->getInverseInertiaTensorWorld(&inverseInertiaTensor[0]);
	if(body[1])
	{
		body[1]->getInertiaTensorWorld(&inverseInertiaTensor[1]);
	}

	//This holds the value for the calculated impulse for each contact axis.
	Vector3 impulseContact;

	if(friction == (real)0.0)
	{
		impulseContact = calculateFrictionlessImpulse(inverseInertiaTensor);
	}
	else
	{
		impulseContact = calculateFrictionImpulse(inverseInertiaTensor);
	}

	//We get the impulse in world coordinates
	Vector3 impulseWorld = contactToWorld.transform(impulseContact);

	//Next we split up the rotational and linear components and apply them.
	Vector3 impulsiveTorque = relativeContactPosition[0] % impulseWorld;
	rotationChange[0] = inverseInertiaTensor[0].transform(impulsiveTorque);
	velocityChange[0].Clear();
	velocityChange[0].addScaledVector(impulseWorld, body[0]->getInverseMass());

	//Here we apply the change.
	body[0]->addVelocity(velocityChange[0]);
	body[0]->addRotation(rotationChange[0]);

	if(body[1])
	{
		//Next we split up the rotational and linear components and apply them.
		Vector3 impulsiveTorque = impulseWorld % relativeContactPosition[1];
		rotationChange[1] = inverseInertiaTensor[1].transform(impulsiveTorque);
		velocityChange[1].Clear();
		velocityChange[1].addScaledVector(impulseWorld, -body[1]->getInverseMass());

		//Here we apply the change.
		body[1]->addVelocity(velocityChange[1]);
		body[1]->addRotation(rotationChange[1]);
	}
}

void Contact::applyPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], real penetration)
{
	const real angularLimit = (real)0.2;
    real angularMove[2];
    real linearMove[2];

    real totalInertia = 0;
    real linearInertia[2];
    real angularInertia[2];

	//Firstly we need to workout the interia in the direction of the contact normal.
	for(unsigned i = 0; i < 2; i++)
	{
		if(body[i])
		{
			Matrix3 inverseInertiaTensor;
			body[i]->getInverseInertiaTensorWorld(&inverseInertiaTensor);

			//Here we get the angular interia in world basis
			Vector3 angularInertiaWorld = relativeContactPosition[i] % contactNormal;
			angularInertiaWorld = inverseInertiaTensor.transform(angularInertiaWorld);
			angularInertiaWorld = angularInertiaWorld % relativeContactPosition[i];
			angularInertia[i] = angularInertiaWorld * contactNormal;

			//Here we get the linear compoment.
			linearInertia[i] = body[i]->getInverseMass();

			totalInertia += linearInertia[i] + angularInertia[i];
		}
	}

	//After we have the interia we work through applying changes.
	for(int i = 0; i < 2; i++)
	{
		if(body[i])
		{
			//We need a proportion that might inverse or not the tensors.
			real sign = (i == 0) ? 1 : -1;
			angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
			linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);

			//Here we need to slow down the rotation only if the mass is great and the interia is low
			Vector3 projection = relativeContactPosition[i];
			projection.addScaledVector(contactNormal, -relativeContactPosition[i].scalarProduct(contactNormal));

			//Here we use the small angle approximation for the sine of the angle (i.e the magnitude would be sine(angularLimit) * projection.magnitude
        	//but we approximate sine(angularLimit) to angularLimit).
			real maxMagnitude = angularLimit * projection.magnitude();

			if(angularMove[i] < -maxMagnitude)
			{
				real totalMove = angularMove[i] + linearMove[i];
				angularMove[i] = -maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}
			else if(angularMove[i] > maxMagnitude)
			{
				real totalMove = angularMove[i] + linearMove[i];
				angularMove[i] = maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}

			//If there is no rotation we don't need to worry about it
			if(angularMove[i] == 0)
			{
				angularChange[i].Clear();
			}
			else
			{
				//Here we find the angle we want to rotation in.
				Vector3 targetedAngularDirection = relativeContactPosition[i].vectorProduct(contactNormal);

				Matrix3 inverseInertiaTensor;
				body[i]->getInverseInertiaTensorWorld(&inverseInertiaTensor);

				//Now we workout the rotation in the direction we needed it in.
				angularChange[i] = inverseInertiaTensor.transform(targetedAngularDirection) * (angularMove[i] / angularInertia[i]);
			}

			//Next we work out the total linear change.
			linearChange[i] = contactNormal * linearMove[i];

			//Next we apply the changes we have calculated.
			Vector3 pos;
			body[i]->getPosition(&pos);
			pos.addScaledVector(contactNormal, linearMove[i]);
			body[i]->setPosition(pos);

			//Next we do the same for angular rotation.
			Quaternion q;
			body[i]->getOrientation(&q);
			q.addScaledVector(angularChange[i], 1.0);
			body[i]->setOrientation(q);

			//Finally we need to calculate the devired data of the rigid bodies.
			//Also we check to see if things are awake.
			if(!body[i]->getAwake())
			{
				body[i]->calculateDerivedData();
			}
		}
	}
}

void Contact::calculateDesiredDeltaVelocity(real duration)
{
	const static real velocityLimit = (real)0.25;

	//Calculate the acceleration induced velocity accumulated this frame
    real velocityFromAcc = 0;

	//Here we calculate the desired velocity of the bodies to get the seperation after contact.
	if(body[0]->getAwake())
	{
		velocityFromAcc += body[0]->getLastFrameAcceleration() * duration * contactNormal;
	}

	if(body[1] && body[0]->getAwake())
	{
		velocityFromAcc -= body[1]->getLastFrameAcceleration() * duration * contactNormal;
	}

	//If the velcity is slow we need to limit the bounce of the objects.
	//This is so we are not bouncing forever making this unstable.
	real thisResititution = restitution;
	if(std::abs(contactVelocity.x) < velocityLimit)
	{
		thisResititution = (real)0.0;
	}

	//Finally we get the desired velocity. This is the result of the algorithm.
	desiredChangedVelocity = -contactVelocity.x - thisResititution * (contactVelocity.x - velocityFromAcc);
}

Vector3 Contact::calculateLocalVelocity(unsigned int index, real duration)
{
    RigidBody *thisBody = body[index];

    //Here we work out the velocity of the contact point.
    Vector3 velocity = thisBody->getRotation() % relativeContactPosition[index];
	velocity += thisBody->getVelocity();

    //Next we turn the velocity into contact coordinates.
    Vector3 contactVelocity = contactToWorld.transformTranspose(velocity);

    //Next we workout the amount of velocity that is due to forces without reactions.
    Vector3 forVelocity = thisBody->getLastFrameAcceleration() * duration;

	//Again we need to turn this accelertation into world coordinates.
	forVelocity = contactToWorld.transformTranspose(forVelocity);

	//We are only interested in planar velocity so we don't need the x value.
	forVelocity.x = 0;

	//Here we add the planar velocities which will be removed if there is not enough friction.
	contactVelocity += forVelocity;

    return contactVelocity;
}

inline Vector3 Contact::calculateFrictionImpulse(Matrix3* inverseInertiaTensor)
{
	Vector3 impulseContact;
	real inverseMass = body[0]->getInverseMass();

	//Firstly we get the skew-symmetric matrix.
	Matrix3 impulseToTorque;
	impulseToTorque.setSkewSymmetric(relativeContactPosition[0]);

	//Next we build the matrix to convert contact impulse to change in velocity in world coordinates.
	Matrix3 deltaVelWorld = impulseToTorque;
	deltaVelWorld *= inverseInertiaTensor[0];
	deltaVelWorld *= impulseToTorque;
	deltaVelWorld *= -1;

	//Next we replease the process for the second body
	if(body[1])
	{
		impulseToTorque.setSkewSymmetric(relativeContactPosition[1]);

		//Next we build the matrix to convert contact impulse to change in velocity in world coordinates.
		Matrix3 deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 *= inverseInertiaTensor[1];
		deltaVelWorld2 *= impulseToTorque;
		deltaVelWorld2 *= -1;

		//If there is 2 bodies we add the new delta velocity in world space to the orginal.
		deltaVelWorld += deltaVelWorld2;

		//And we need to add the new inverse mass to.
		inverseMass += body[1]->getInverseMass();
	}

	//After that we need to preform a change of the basis to convert into contact coordinates.
	Matrix3 deltaVelocity = contactToWorld.transpose();
	deltaVelocity *= deltaVelWorld;
	deltaVelocity *= contactToWorld;

	//Next we add the linear velocity change.
	deltaVelocity.data[0][0] += inverseMass;
	deltaVelocity.data[1][1] += inverseMass;
	deltaVelocity.data[2][2] += inverseMass;

	//Now we work out the impulse needed per unit
	Matrix3 impulseMatrix = deltaVelocity.inverse();

	//Find the target velocities to kill.
	Vector3 velKill(desiredChangedVelocity, -contactVelocity.y, -contactVelocity.z);

	//After that the impulse to kill target velocities.
	impulseContact = impulseMatrix.transform(velKill);

	//Check for exceeding friction.
	real planarImpulse = std::sqrt(impulseContact.y * impulseContact.y + impulseContact.z * impulseContact.z);
	if(planarImpulse > impulseContact.x * friction)
	{
		//We need to use dynamic friction.
		impulseContact.y /= planarImpulse;
		impulseContact.z /= planarImpulse;
		impulseContact.x = deltaVelocity.data[0][0] + deltaVelocity.data[0][1] * friction * impulseContact.y + deltaVelocity.data[0][2] * friction * impulseContact.z;
		impulseContact.x = desiredChangedVelocity / impulseContact.x;
		impulseContact.y *= friction * impulseContact.x;
		impulseContact.z *= friction * impulseContact.x;
	}

	return impulseContact;
}

ContactResolver::ContactResolver(unsigned interations, real velocityEpsilon, real positionEpsilon) :
    positionIterations(interations),
    velocityIterations(interations),
    velocityEpsilon(velocityEpsilon),
    positionEpsilon(positionEpsilon)
{
}

ContactResolver::ContactResolver(unsigned positionIterations, unsigned velocityIterations, real velocityEpsilon, real positionEpsilon) :
    positionIterations(positionIterations),
    velocityIterations(velocityIterations),
    velocityEpsilon(velocityEpsilon),
    positionEpsilon(positionEpsilon)
{
}

void ContactResolver::setEpsilon(const real &velocityEpsilon, const real &positionEpsilon)
{
	 ContactResolver::velocityEpsilon = velocityEpsilon;
	 ContactResolver::positionEpsilon = positionEpsilon;
}

void ContactResolver::prepareContacts(Contact* contactArray, unsigned int numContacts, real duration)
{
    for(Contact* contact = contactArray; contact < contactArray + numContacts; contact++)
    {
        //Here we calculate all the contact data stuff.
        contact->calculateInternals(duration);
    }
}

void ContactResolver::adjustVelocities(Contact* contactArray, unsigned int numContacts, real duration)
{
	Vector3 velocityChange[2], rotationChange[2];
	Vector3 deltaVelocity;
	unsigned index;

	//Like all these algorithms we start with the most severe.
	velocityIterationsUsed = 0;
	while(velocityIterationsUsed < velocityIterations)
    {
        //Here we find the biggest penetration.
        real max = velocityEpsilon;
        index = numContacts;
        for(unsigned i = 0; i < numContacts; i++)
        {
            if(contactArray[i].desiredChangedVelocity > max)
            {
                max = contactArray[i].desiredChangedVelocity;
                index = i;
            }
        }

        if(index == numContacts)
        {
           break;
        }

        contactArray[index].matchAwakeState();

		//Here we do all the hard stuff resolve a collision.
		contactArray[index].applyVelocityChange(velocityChange, rotationChange);

		//Here we updated the ctacts when the penereation has changed run again
		for(unsigned i = 0; i < numContacts; i++)
		{
			//check each body in the contact
			for(unsigned j = 0; j < 2; j++)
			{
				//Check to see if the the other body is live and to see with which body needs to be resolved.
				if(contactArray[i].body[j])
				{
					for(unsigned b = 0; b < 2; b++)
					{
						if(contactArray[i].body[j] == contactArray[index].body[b])
						{
							deltaVelocity = velocityChange[b] + rotationChange[b].vectorProduct(contactArray[i].relativeContactPosition[j]);

							// The sign of the change is negative if we're dealing with the second body in a contact.
							contactArray[i].contactVelocity += contactArray[i].contactToWorld.transformTranspose(deltaVelocity) * (j ? -1 : 1);

							contactArray[i].calculateDesiredDeltaVelocity(duration);
						}
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}

void ContactResolver::adjustPositions(Contact* contactArray, unsigned int numContacts, real duration)
{
    unsigned i, index;
    Vector3 linearChange[2], angularChange[2];
    real max;
    Vector3 deltaPosition;

    //Here we start to interatively resolve interpenetrations in order of severity.
    positionIterationsUsed = 0;
    while(positionIterationsUsed < positionIterations)
    {
        //Here we find the biggest penetration.
        max = positionEpsilon;
        index = numContacts;
        for(i = 0; i < numContacts; i++)
        {
            if(contactArray[i].penetration > max)
            {
                max = contactArray[i].penetration;
                index = i;
            }
        }
        if(index == numContacts)
        {
           break;
        }

        contactArray[index].matchAwakeState();

		//Here we do all the hard stuff resolve a collision.
		contactArray[index].applyPositionChange(linearChange, angularChange, max);

		//Here we updated the ctacts when the penereation has changed run again
		for(i = 0; i < numContacts; i++)
		{
			//check each body in the contact
			for(int j = 0; j < 2; j++)
			{
				//Check to see if the the other body is live and to see with which body needs to be resolved.
				if(contactArray[i].body[j])
				{
					for(int b = 0; b < 2; b++)
					{
						if(contactArray[i].body[j] == contactArray[index].body[b])
						{
							deltaPosition = linearChange[b] + angularChange[b].vectorProduct(contactArray[i].relativeContactPosition[j]);

							//If the sign of the change id positive if we're dealing the second body other wise negative.
							contactArray[i].penetration += deltaPosition.scalarProduct(contactArray[i].contactNormal) * (j ? 1 : -1);
						}
					}
				}
			}
		}
		positionIterationsUsed++;
	}
}

void ContactResolver::resolveContact(Contact* contactArray, unsigned int numContacts, real duration)
{
    //Firstly we need to check if there is any contacts if not we exit.
    if(numContacts == 0)
    {
        return;
    }

	if(!isValid())
	{
		return;
	}

    //Next we need to prepare the collisions for processing.
    //In this case we will use the inline preprocessing functions.
    prepareContacts(contactArray, numContacts, duration);

	//After that we need to work through the interpenetration problems with the contacts.
    adjustPositions(contactArray, numContacts, duration);

    //Finally we resolve the velocity problems with the contacts.
    adjustVelocities(contactArray, numContacts, duration);
}
