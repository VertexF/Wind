#include "collision_narrow.h"

using namespace wind;

/**Inline functions*/
inline real transformToAxis(const Box& box, const Vector3& axis)
{
    return((box.halfSize.x * std::abs(axis * box.getAxis(0))) +
           (box.halfSize.y * std::abs(axis * box.getAxis(1))) +
           (box.halfSize.z * std::abs(axis * box.getAxis(2))));
}

//This function checks to see if there is an axis overlap so over 15 axis.
inline real penetrationOnAxis(const Box& first, const Box& second, const Vector3& axis, const Vector3& toCentre)
{
     real firstProject = transformToAxis(first, axis);
     real secondProject = transformToAxis(second, axis);

     real distance = std::abs(toCentre * axis);

     //Here we return the overlap
     return (firstProject + secondProject - distance);
}

inline bool overlapOnAxis(const Box& first, const Box& second, const Vector3& axis, const Vector3& toCentre)
{
    real projectionOne = transformToAxis(first, axis);
    real projectionTwo = transformToAxis(second, axis);

    real distance = std::abs(toCentre * axis);

    return(distance < projectionOne + projectionTwo);
}

inline bool tryAxis(const Box& first, const Box& second, Vector3 axis, const Vector3& toCentre, unsigned index, real& smallestPenetration, unsigned& smallestCase)
{
    if(axis.squareMagnitude() < 0.0001)
    {
        return true;
    }
    axis.normalise();

    real penetration = penetrationOnAxis(first, second, axis, toCentre);

    if(penetration < 0)
    {
        return false;
    }

    if(penetration < smallestPenetration)
    {
        smallestPenetration = penetration;
        smallestCase = index;
    }

    return true;
}

inline Vector3 getContactPoint(const Vector3& ptOnEdgeOne, const Vector3& firstAxis, real firstHalfSize, const Vector3& ptOnEdgeTwo, const Vector3& secondAxis, real secondHalfSize,
                                      /*If this is true we have gone out of bonds*/ bool useOne)
{
    //Firstly we need a test point.
    Vector3 toSt = ptOnEdgeOne - ptOnEdgeTwo;

    //Now we need to work out in which direction of each edge.
    real dpStaOne = firstAxis * toSt;
    real dpStaTwo = secondAxis * toSt;

    //Here we workout out how far along each edge is the closest point.
    real smFirst = firstAxis.squareMagnitude();
    real smSecond = secondAxis.squareMagnitude();
    real dotProductEdges = firstAxis * secondAxis;

    real denom = smFirst * smSecond - dotProductEdges * dotProductEdges;

    //Here we just check to see if the edges are parrallel lines.
    if(std::abs(denom) < 0.00001f)
    {
        return useOne ? ptOnEdgeOne : ptOnEdgeTwo;
    }

    real a = (dotProductEdges * dpStaTwo - smSecond * dpStaOne) / denom;
    real b = (smFirst * dpStaTwo - dotProductEdges * dpStaOne) / denom;

    //Here we check to see if the any of the nearest points are out of bounds of the edges contact.

    if(a > firstHalfSize || a < -firstHalfSize || b > secondHalfSize || b < -secondHalfSize)
    {
        return useOne ? ptOnEdgeOne : ptOnEdgeTwo;
    }
    else
    {
        //Next we use the a point midway between the two nearest point.
        Vector3 nearestPtOnOne = ptOnEdgeOne + firstAxis * a;
        Vector3 nearestPtOnTwo = ptOnEdgeTwo + secondAxis * b;

        return(nearestPtOnOne * 0.5 + nearestPtOnTwo * 0.5);
    }
}

void Primitive::calculateInternals()
{
	if(body == nullptr)
	{
		std::cerr << "The bodies primitive is null" << std::endl;
		return;
	}

    transform = body->getTransform();// * offset;
}

bool IntersectionTests::BoxAndHalfSpace(const Box& box, const Plane& plane)
{
    //First to see if a box is colliding in halfspace we first need to work out the projectedRadius
    real projectedRadius = transformToAxis(box, plane.direction);

    //Next we get the box direction.
    real boxDirection = (plane.direction * box.getAxis(3) - projectedRadius);

    return(boxDirection <= plane.offset);
}

//Here we are checking the axes on the fly so we can avoid wasting processor time.
bool IntersectionTests::BoxAndBox(const Box& first, const Box& second)
{

    Vector3 toCentre = second.getAxis(3) - first.getAxis(3);

    auto overlap_test = [&] (Vector3 axis) { return(overlapOnAxis(first, second, axis, toCentre)); };

    return(
    //Here we get the axes of the first objects faces and only the non-parallel side.
    overlap_test(first.getAxis(0)) && overlap_test(first.getAxis(1)) && overlap_test(first.getAxis(2)) &&
    //We do the same for the second object.
    overlap_test(second.getAxis(0)) && overlap_test(second.getAxis(1)) && overlap_test(second.getAxis(2)) &&

    //Next we get the axes for the edges.
    //Remember % is a vector cross product it has  nothing to do with modula calculations.
    overlap_test(first.getAxis(0) % second.getAxis(0)) && overlap_test(first.getAxis(0) % second.getAxis(1)) &&
    overlap_test(first.getAxis(0) % second.getAxis(2)) && overlap_test(first.getAxis(1) % second.getAxis(0)) &&
    overlap_test(first.getAxis(1) % second.getAxis(1)) && overlap_test(first.getAxis(1) % second.getAxis(2)) &&
    overlap_test(first.getAxis(2) % second.getAxis(0)) && overlap_test(first.getAxis(2) % second.getAxis(1)) &&
    overlap_test(first.getAxis(2) % second.getAxis(2))
    );
}

bool IntersectionTests::SphereAndHalfSpace(const Sphere& sphere, const Plane& plane)
{
	//Here we find the distance between the orgin point and sphere.
	real ballDistance = plane.direction * sphere.getAxis(3) - sphere.radius;

	//Now we check for the intersection
    return ballDistance <= plane.offset;
}

bool IntersectionTests::SphereAndSphere(const Sphere& first, const Sphere& second)
{
	//Firstly we find the vector between the objects
    Vector3 midline = first.getAxis(3) - second.getAxis(3);

    // See if it is large enough.
    return midline.squareMagnitude() < (first.radius + second.radius) * (first.radius + second.radius);
}

unsigned CollisionDetection::SphereAndSphere(const Sphere& first, const Sphere& second, CollisionData& data)
{
    unsigned int contact = 0;
    if (data._contactsLeft > 0)
    {
        //Here we get the position of the spheres.
        Vector3 positionOne = first.getAxis(3);
        Vector3 positionTwo = second.getAxis(3);

        //Here we get the vector between the points.
        Vector3 midline = positionOne - positionTwo;
        real Size = midline.magnitude();

        //Here we see if the midline is big enough or that the sphere are intersecting.
        if (Size > 0.f || Size < first.radius + second.radius)
        {
            //Next we generate the contact normal because we have an collision.
            Vector3 contactNormal = midline * (((real)1.0) / Size);

            Contact* contact = data._contacts;
            contact->contactNormal = contactNormal;
            contact->contactPoint = positionOne + midline * (real)0.5;
            contact->penetration = (first.radius + second.radius - Size);
            contact->setContactData(first.body, second.body, data._friction, data._restitution);

            data.addContact(1);
            contact++;
        }
    }

    return contact;
}

unsigned CollisionDetection::SphereAndHalfSpace(const Sphere& sphere, const Plane& plane, CollisionData& data)
{
    unsigned int contact = 0;
    if (data._contactsLeft > 0)
    {

        //Here we are getting the position of the sphere.
        Vector3 position = sphere.getAxis(3);

        //Next we find the distance between the ball than and the sphere.
        real ballDistance = plane.direction * position - sphere.radius - plane.offset;

        if (ballDistance < 0)
        {
            //There is a collision we now need to create the contact data.
            //Which will have a normal in the plane direction.
            Contact* contact = data._contacts;
            contact->contactNormal = plane.direction;
            contact->penetration = -ballDistance;
            contact->contactPoint = position - plane.direction * (ballDistance + sphere.radius);
            contact->setContactData(sphere.body, nullptr, data._friction, data._restitution);

            data.addContact(1);
            contact++;
        }
    }

    return contact;
}

unsigned CollisionDetection::SphereAndTruePlane(const Sphere& sphere, const Plane& plane, CollisionData& data)
{
    unsigned int contact = 0;
    if (data._contactsLeft > 0)
    {
        //Here we are getting the position of the sphere.
        Vector3 position = sphere.getAxis(3);

        //Next we calculate the distance between the plane and the sphere.
        real centreDistance = plane.direction * position - plane.offset;

        //Check to see if the radius is within the plane.
        if (centreDistance * centreDistance <= sphere.radius * sphere.radius)
        {
            //Now we need to check which side of the plane are we on.
            Vector3 normal = plane.direction;
            real penetration = -centreDistance;

            if (centreDistance < 0)
            {
                normal *= -1;
                penetration = -penetration;
            }

            penetration += sphere.radius;

            //Now we create a contact normal.
            Contact* contact = data._contacts;
            contact->contactNormal = normal;
            contact->penetration = penetration;
            contact->contactPoint = position - plane.direction * centreDistance;
            contact->setContactData(sphere.body, nullptr, data._friction, data._restitution);

            data.addContact(1);
            contact++;
        }
    }
}

unsigned CollisionDetection::BoxAndHalfSpace(const Box& box, const Plane& plane, CollisionData& data)
{
    unsigned contactsUsed = 0;
    //Here we check to see if the box is intersecting with the halfspace.
    if (data._contactsLeft > 0 && IntersectionTests::BoxAndHalfSpace(box, plane))
    {
        //Okay so the box has collided with the halfspace now we need to fine the intersection points.
        //So if the box is resting on a plane there are either 2 or 4 vertices touching.

        //Here we go through each combination of + and - for each half size.
        real mults[8][3] = { {1, 1, 1}, {-1, 1, 1}, {1, -1, 1}, {-1, -1, 1},
                            {1, 1, -1}, {-1, 1, -1}, {1, -1, -1}, {-1, -1, -1} };

        Contact* contact = data._contacts;
        for (int i = 0; i < 8; i++)
        {
            //First we need to calculate the position of each vertex point.
            Vector3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
            vertexPos.componentProductUpdate(box.halfSize);
            vertexPos = box.transform.transform(vertexPos);

            //Next we calculate the distance from the plane to the box.
            real vertexDistance = vertexPos * plane.direction;

            //Next we compare this plane's distance.
            if (vertexDistance <= plane.offset && contactsUsed == static_cast<unsigned>(data._contactsLeft))
            {
                //Here we are creating the contact data.

                //The contact is halfway between the vertex the plane. We multiply the direction by hold the separation distance and as the vertex
                contact->contactPoint = plane.direction;
                contact->contactPoint *= (vertexDistance - plane.offset);
                contact->contactPoint = vertexPos;
                contact->contactNormal = plane.direction;
                contact->penetration = plane.offset - vertexDistance;

                //Here we write the appropriate data.
                contact->setContactData(box.body, nullptr, data._friction, data._restitution);

                //Like normal we move onto the next contact.
                contact++;
                contactsUsed++;
            }
        }

        data.addContact(contactsUsed);
    }

    return contactsUsed;
}

unsigned CollisionDetection::BoxAndSphere(const Box& box, const Sphere& sphere, CollisionData& data)
{
    unsigned int contact = 0;
    if (data._contactsLeft > 0)
    {
        Vector3 closestPointWorld(0.0, 0.0, 0.0);

        //Here we first get the centre of the sphere and box.
        Vector3 centre = sphere.getAxis(3);
        Vector3 relCentre = box.transform.transformInverse(centre);

        //This if statement is to see if there is contact if so we return.
        if (std::abs(relCentre.x) - sphere.radius <= box.halfSize.x ||
            std::abs(relCentre.y) - sphere.radius <= box.halfSize.y ||
            std::abs(relCentre.z) - sphere.radius <= box.halfSize.z)
        {

            Vector3 closestPoint(0, 0, 0);
            real dist;

            //Next we clamp each of the coordinates.
            dist = relCentre.x;
            if (dist > box.halfSize.x)
            {
                dist = box.halfSize.x;
            }

            if (dist < -box.halfSize.x)
            {
                dist = -box.halfSize.x;
            }
            closestPoint.x = dist;

            dist = relCentre.y;
            if (dist > box.halfSize.y)
            {
                dist = box.halfSize.y;
            }

            if (dist < -box.halfSize.y)
            {
                dist = -box.halfSize.y;
            }
            closestPoint.y = dist;

            dist = relCentre.z;
            if (dist > box.halfSize.z)
            {
                dist = box.halfSize.z;
            }

            if (dist < -box.halfSize.z)
            {
                dist = -box.halfSize.z;
            }
            closestPoint.z = dist;

            //Here we check to see if we are contacting.
            dist = (closestPoint - relCentre).squareMagnitude();
            if (dist <= sphere.radius * sphere.radius)
            {

                //Here we need to get the closest points in world space.
                closestPointWorld = box.transform.transform(closestPoint);

                Contact* contact = data._contacts;
                contact->contactNormal = (closestPointWorld - centre);
                contact->contactNormal.normalise();
                contact->contactPoint = closestPointWorld;
                contact->penetration = sphere.radius - std::sqrt(dist);
                contact->setContactData(box.body, sphere.body, data._friction, data._restitution);

                data.addContact(1);
                contact++;
            }
        }
    }

    return contact;
}

unsigned CollisionDetection::BoxAndPoint(const Box &box, const Vector3 &point, CollisionData& data)
{
    // Firstly we transform the point into box coordinates
    Vector3 relPt = box.transform.transformInverse(point);

    //Here we are checking each axis to see if the which as has the least penetration.
    //Here we are getting the absolute position of the x position.
    real minDepth = box.halfSize.x - std::abs(relPt.x);
    //If the minimum depth is negative then we return no contact.
    if(minDepth < 0)
    {
        return 0;
    }
    //At this point we have the minimum penetration so we need to generate the contact normal.
    Vector3 normal = box.getAxis(0) * ((relPt.x < 0) ? -1 : 1);

    //We do this for each axis.
    real depth = box.halfSize.y - std::abs(relPt.y);
    if(depth < 0)
    {
        return 0;
    }
    //Each time the depth is smaller than the minimum depths we generate a new contact normal and set the minDepth to the current depth.
    else if (depth < minDepth)
    {
        minDepth = depth;
        normal = box.getAxis(1) * ((relPt.y < 0) ? -1 : 1);
    }

    depth = box.halfSize.z - std::abs(relPt.z);
    if(depth < 0)
    {
        return 0;
    }
    else if (depth < minDepth)
    {
        minDepth = depth;
        normal = box.getAxis(2) * ((relPt.z < 0) ? -1 : 1);
    }

    // Compile the contact
    Contact* contact = data._contacts;
    contact->contactNormal = normal;
    contact->contactPoint = point;
    contact->penetration = minDepth;

    contact->setContactData(box.body, nullptr, data._friction, data._restitution);

    data.addContact(1);
    return 1;
}

//This is a helper function for the box on box collision.
//This function is only called when the boxes are in contact with each other.
void fillPointBoxOnBox(const Box& first, const Box& second, const Vector3& toCentre, CollisionData& data, unsigned best, real penetration)
{
    Contact* contact = data._contacts;

    //Okay currently we need which axis the collision has happened one but we need to work out which face that axis is on.
    Vector3 normal = first.getAxis(best);
    if(first.getAxis(best) * toCentre > 0)
    {
        normal = normal * -1.0;
    }

    //Now here we work out which face is contacting with what.
    Vector3 vertex = second.halfSize;
    if(second.getAxis(0) * normal < 0)
    {
        vertex.x = -vertex.x;
    }
    if(second.getAxis(1) * normal < 0)
    {
        vertex.y = -vertex.y;
    }
    if(second.getAxis(2) * normal < 0)
    {
        vertex.z = -vertex.z;
    }

    //Here we change the vertex into work coordinates.
    vertex = second.getTransform() * vertex;

    contact->contactNormal = normal;
    contact->penetration = penetration;
    contact->contactPoint = vertex;
    contact->setContactData(first.body, second.body, data._friction, data._restitution);
}

//Here we are checking weather or not there is an overlap.
unsigned CollisionDetection::BoxAndBox(const Box& first, const Box& second, CollisionData& data)
{
    if(!IntersectionTests::BoxAndBox(first, second))
	{
        return 0;
    }

    Vector3 toCentre = second.getAxis(3) - first.getAxis(3);

    //So what this does is it assumes there is no contact.
    //This is why we have a huge penetration depth and the max unsigned value.
    real penetration = std::numeric_limits<real>::max();
    unsigned bestCase = 0xffffff;

    auto overlap_check = [&] (Vector3 axis, unsigned index) {if(!tryAxis(first, second, axis, toCentre, index, penetration, bestCase)){return 0;}};

    //Next we check to each axes, returning if it gives us a separating axis.
    //Here we do the faces of the square.
    overlap_check(first.getAxis(0), 0);
    overlap_check(first.getAxis(1), 1);
    overlap_check(first.getAxis(2), 2);

    overlap_check(second.getAxis(0), 3);
    overlap_check(second.getAxis(1), 4);
    overlap_check(second.getAxis(2), 5);

    //Store the best axis-major, in case we run into almost
    //parallel edge collisions later
    unsigned bestSingleAxis = bestCase;

    //Next we do the edges of the boxes.
    overlap_check(first.getAxis(0) % second.getAxis(0), 6);
    overlap_check(first.getAxis(0) % second.getAxis(1), 7);
    overlap_check(first.getAxis(0) % second.getAxis(2), 8);
    overlap_check(first.getAxis(1) % second.getAxis(0), 9);
    overlap_check(first.getAxis(1) % second.getAxis(1), 10);
    overlap_check(first.getAxis(1) % second.getAxis(2), 11);
    overlap_check(first.getAxis(2) % second.getAxis(0), 12);
    overlap_check(first.getAxis(2) % second.getAxis(1), 13);
    overlap_check(first.getAxis(2) % second.getAxis(2), 14);

    assert(bestCase != 0xffffff);

    //In this stage of the algorithm we know that there is a collision and how many axes have given the smallest penetration.
    //Now there are afew different ways to deal with it.
    if(bestCase < 3)
    {
        fillPointBoxOnBox(first, second, toCentre, data, bestCase, penetration);
        data.addContact(1);
        return 1;
    }
    else if(bestCase < 6)
    {
        //Here we are inversing the toCentre vector of the box and setting up the bestCase to find out which point is touching a face.
        toCentre = toCentre * -1.f;
        bestCase = bestCase - 3;

        fillPointBoxOnBox(first, second, toCentre, data, bestCase, penetration);
        data.addContact(1);
        return 1;
    }
    else
    {
        //Here we check the edge on edge collision first we need to set up data.
        bestCase -= 6;
        //These variable allow us to find out what overlapping axes we have.
        unsigned firstAxisIndex = bestCase / 3;
        unsigned secondAxisIndex = bestCase % 3;
        //Here we are storing the two axis for the SAT to work.
        Vector3 firstAxis = first.getAxis(firstAxisIndex);
        Vector3 secondAxis = second.getAxis(secondAxisIndex);
        //To get the axis we need to use for SAT we need to use the vector cross product.
        Vector3 SATAxis = firstAxis % secondAxis;
        //Finally we normalise it to get the normal of the edge of the contact.
        //Which of course is our contact normal!
        SATAxis.normalise();

        if(SATAxis * toCentre > 0)
        {
            SATAxis = SATAxis * -1.f;
        }

        //Okay now we need to figure out which edge we are using, there can be only 4 edges.
        //We choose which edge by selecting a point on the edges.
        Vector3 ptOnEdgeOne = first.halfSize;
        Vector3 ptOnEdgeTwo = second.halfSize;
        for(unsigned i = 0; i < 3; i++)
        {
            if(i == firstAxisIndex)
            {
                ptOnEdgeOne[i] = 0;
            }
            else if(first.getAxis(i) * SATAxis > 0)
            {
                ptOnEdgeOne[i] = -ptOnEdgeOne[i];
            }

            if(i == secondAxisIndex)
            {
                ptOnEdgeTwo[i] = 0;
            }
            else if(second.getAxis(i) * SATAxis < 0)
            {
                ptOnEdgeTwo[i] = -ptOnEdgeTwo[i];
            }
        }

        ptOnEdgeOne = first.transform * ptOnEdgeOne;
        ptOnEdgeTwo = second.transform * ptOnEdgeTwo;

        //Here we get the contact point of the edge to edge contact.
        Vector3 vertex = getContactPoint(ptOnEdgeOne, firstAxis, first.halfSize[firstAxisIndex],
											ptOnEdgeTwo, secondAxis, second.halfSize[secondAxisIndex], bestSingleAxis > 2);

        //Now we create a contact normal.
        Contact* contact = data._contacts;
        contact->contactNormal = SATAxis;
        contact->penetration = penetration;
        contact->contactPoint = vertex;
        contact->setContactData(first.body, second.body, data._friction, data._restitution);

        data.addContact(1);
        return 1;
    }

    return 0;
}