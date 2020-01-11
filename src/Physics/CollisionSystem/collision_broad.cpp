#include "collision_broad.h"

using namespace wind;

BoundingSphere::BoundingSphere(const Vector3 &centre, real radius) : centre(centre), radius(radius)
{
}

BoundingSphere::BoundingSphere(const BoundingSphere &first, const BoundingSphere &second)
{
    //Firstly we calculate the centre between both spheres.
    Vector3 centreOffset = first.centre - second.centre;
    //Next we work out the difference length between between both spheres.
    real distance = centreOffset.squareMagnitude();
    //After that we work out the difference between the two radii.
    real radiusDifference = second.radius - first.radius;

    //Here we are checking to see if one of the sphere is inside the other.
    if(radiusDifference * radiusDifference >= distance)
    {
        if(first.radius > second.radius)
        {
            centre = first.centre;
            radius = first.radius;
        }
        else
        {
            centre = second.centre;
            radius = second.radius;
        }
    }
    //If not we need to workout which sphere is overlapping.
    else
    {
        distance = std::sqrt(distance);
        radius = (distance + first.radius + second.radius) * ((real)0.5);

        //Finally we workout the new centre. By basing it off the first sphere centre and move it towards second sphere centre.
        //By the amount proportional to the sphere's radii.
        centre = first.centre;
        if(distance > 0)
        {
            centre += centreOffset * ((radius - first.radius) / distance);
        }
    }
}

bool BoundingSphere::overlap(const BoundingSphere *other) const
{
    real distanceSqrt = (centre - other->centre).squareMagnitude();

    return (distanceSqrt < (radius + other->radius) * (radius + other->radius));
}

real BoundingSphere::getGrowth(const BoundingSphere &other) const
{
    BoundingSphere newSphere(*this, other);

    return(newSphere.radius * newSphere.radius - radius * radius);
}

template<class BoundingVolumeClass>
BondVolumeNode<BoundingVolumeClass>::~BondVolumeNode()
{
    //If we don't have a parent then we ignore the sibling
    if(parent)
    {
        BondVolumeNode<BoundingVolumeClass> sibling = new BondVolumeNode<BoundingVolumeClass>;
        if(parent->child[0] == this)
        {
            sibling = parent->child[1];
        }
        else
        {
            sibling = parent->child[0];
        }

        //Next we write this to the parent node.
        parent->volume = sibling->volume;
        parent->body = sibling->body;
        parent->child[0] = sibling->child[0];
        parent->child[1] = sibling->child[1];

        sibling->parent = nullptr;
        sibling->body = nullptr;

        parent->recalculatesBoundingVolume();
    }
}

template<class BoundingVolumeClass>
BondVolumeNode<BoundingVolumeClass>::BondVolumeNode(BondVolumeNode *parent, const BoundingVolumeClass &volume, RigidBody* body)
    : parent(parent), volume(volume), body(body)
{
    child[0] = nullptr;
    child[1] = nullptr;
}


//Check to see if we are at the bottom of the tree node.
template<class BoundingVolumeClass>
bool BondVolumeNode<BoundingVolumeClass>::isLeaf() const
{
    return (body != nullptr);
}

//This functions will be using templates so that they can be implemented in anyway needed.
template<class BoundingVolumeClass>
bool BondVolumeNode<BoundingVolumeClass>::overlaps(const BoundingVolumeClass* other)
{
    return volume->overlaps(other->volume);
}

template<class BoundingVolumeClass>
unsigned BondVolumeNode<BoundingVolumeClass>::getPotentialContact(PotentialContact* contacts, unsigned int limit) const
{
     //If this node is the current leaf node or the limit of contacts is hit then we return 0
     //This is because we either we don't have room for this contact or we are on the leaf node.
     if(isLeaf() || limit == 0)
     {
         return 0;
     }

     //Here we are returning one of the potential contacts between child 1 and 0
     return child[0]->getPotentialContact(child[1], contacts, limit);
}

template<class BoundingVolumeClass>
unsigned BondVolumeNode<BoundingVolumeClass>::getPotentialContact(const BoundingVolumeClass* other, PotentialContact* contacts, unsigned int limit) const
{
     //If there is no overlap then there is no need to report a collision.
     if(!overlaps(other) || limit == 0)
     {
        return 0;
     }

    //Here we check are both leaf nodes if they are you return 1 for a potential.
    if(isLeaf() && other->isLeaf())
    {
        contacts->body[0] = body;
        contacts->body[1] = other->body;
        return 1;
    }

    //In this if statement we are trying to work out which if either is a leaf then we decsend the other.
    //If both nodes are not leaves then we use one with the largest size.
    if(other->isLeaf() || (!isLeaf() && volume->getSize() >= other->volume->getSize()))
    {
        //Here we are using recursion.
        unsigned int counter = child[0]->getPotentialContact(other, contacts, limit);

        //If we haven't reached our limit we can do the other side.
        if(counter < limit)
        {
            return (counter + child[1]->getgetPotentialContact(other, contacts + counter, limit - counter));
        }
        else
        {
            return counter;
        }
    }
    else
    {
        //If false we use recursion to go into the other nodes.
        unsigned int counter = getPotentialContact(other->child[0], contacts, limit);

        //If we haven't reached our limit we can do the other side.
        if(counter < limit)
        {
            return (counter + getgetPotentialContact(other->child[1], contacts + counter, limit - counter));
        }
        else
        {
            return counter;
	    }
	}
}

template<class BoundingVolumeClass>
void BondVolumeNode<BoundingVolumeClass>::recalculatesBoundingVolume(bool recursion)
{
    if(isLeaf())
    {
        return;
    }

    volume = BoundingVolumeClass(child[0]->volume, child[1]->volume);

    if(parent)
    {
        parent->recalculatesBoundingVolume(true);
    }
}

template<class BoundingVolumeClass>
void BondVolumeNode<BoundingVolumeClass>::insert(RigidBody* newBody, const BoundingVolumeClass &newVolume)
{
    //If we are at a leaf node then we need to create 2 more children node
    if(isLeaf())
    {
        //So the first child holds a rigid body copied of the this volume which will hold this current object.
        child[0] = BondVolumeNode<BoundingVolumeClass>(this, volume, body);

        //Next we let the second child take control of the new volume which will hold the other body.
        child[1] = BondVolumeNode<BoundingVolumeClass>(this, newVolume, newBody);

        //Now we lose a rigid body because this node is no longer a leaf node.
        this->body = nullptr;

        //We recalculate the bounding volume.
        recalculatesBoundingVolume();
    }
    // If we are not the leaf node we need to workout which child keeps the inserted body. We give it to who ever would grow the least to incorporate it.
    else
    {
        if(child[0]->volume.getGrowth(newVolume) < child[1]->volume.getGrowth(newVolume))
        {
            child[0]->volume.getGrowth(newBody, newVolume);
        }
        else
        {
            child[1]->volume.getGrowth(newBody, newVolume);
        }
    }
}
