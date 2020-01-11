#include "Geometry.h"

using namespace wind;

Geometry::Geometry()
{
}

inline real Geometry::triArea2D(real x1, real y1, real x2, real y2, real x3, real y3)
{
    return(((x1 - x2) * (y2 - y3)) - ((x2 - x3) * (y1 - y2)));
}

Vector3 Geometry::Barycentric(std::vector<Vector3> Points, std::vector<real> &coords)
{
    for(unsigned int i = 0; i < Points.size(); i++)
    {
        Points.at(i).makePoint();
    }

    Vector3 vec1(Points.at(1) - Points.at(0));
    Vector3 vec2(Points.at(2) - Points.at(0));
    Vector3 vec3(Points.at(3) - Points.at(0));

    real d00 = vec1 * vec1;
    real d01 = vec1 * vec2;
    real d11 = vec2 * vec2;
    real d20 = vec3 * vec1;
    real d21 = vec3 * vec2;

    real demon = d00 * d11 - d01 * d01;

    real v = ((d11 * d20 - d01 * d21) / demon);
    real w = ((d00 * d21 - d01 * d20) / demon);

    Vector3 result(v, w, (1.0 - v - w));

    return result;
}

void Geometry::Barycentric(std::vector<Vector3> Points, real &v, real &u, real &w)
{
    for(unsigned int i = 0; i < Points.size(); i++)
    {
        Points.at(i).makePoint();
    }

    //First we need to unnormalised the triangle normal.
    Vector3 m = Vector3(Points[1] - Points[0]) % Vector3(Points[2] - Points[0]);
    // Nominators and one-over-denominator for v and u ratios.
    real nu, nv, ood;
    //Absolute components for determining projection plane.
    real x = std::abs(m.x);
    real y = std::abs(m.y);
    real z = std::abs(m.z);

    if(x >= y && x >= z)
    {
        nu = triArea2D(Points[3].y, Points[3].z, Points[1].y, Points[1].z, Points[2].y, Points[2].z);
        nv = triArea2D(Points[3].y, Points[3].z, Points[2].y, Points[2].z, Points[0].y, Points[0].z);
        ood = 1.0 / m.x;
    }
    else if(y >= x && y >= z)
    {
        nu = triArea2D(Points[3].x, Points[3].z, Points[1].x, Points[1].z, Points[2].x, Points[2].z);
        nv = triArea2D(Points[3].x, Points[3].z, Points[2].x, Points[2].z, Points[0].x, Points[0].z);
        ood = 1.0 / -m.y;
    }
    else
    {
        nu = triArea2D(Points[3].x, Points[3].y, Points[1].x, Points[1].y, Points[2].x, Points[2].y);
        nv = triArea2D(Points[3].x, Points[3].y, Points[2].x, Points[2].y, Points[0].x, Points[0].y);
        ood = 1.0 / m.z;
    }
    u = nu * ood;
    v = nv * ood;
    w = 1.0 - u - v;
}

int Geometry::testPointTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c)
{
    real u, v, w;
    std::vector<Vector3> jack;
    jack.at(0) = a;
    jack.at(1) = b;
    jack.at(2) = c;
    jack.at(3) = p;
    Barycentric(jack, u, v, w);
    return(v >= 0.0 && w >= 0.0 && (v + w) <= 1.0);
}

Plane Geometry::computePlane(Vector3 PointA, Vector3 PointB, Vector3 PointC)
{
    Plane p;
    p.direction = Vector3(PointB - PointA) % Vector3(PointC - PointA);
    p.offset = p.direction * PointA;
    return p;
}

int Geometry::pointFarthestFromEdge(Vector2 a, Vector2 b, Vector2 p[], int n)
{
    // Create edge vector and vector (counterclockwise) perpendicular to it
    Vector2 e = b - a;
    Vector2 eperp(-e.y, e.x);
    // Track index, ‘distance’ and ‘rightmostness’ of currently best point
    int bestIndex = -1;
    float maxVal = std::numeric_limits<float>::min(), rightMostVal = std::numeric_limits<float>::min();
    // Test all points to find the one farthest from edge ab on the left side
    for(int i = 1; i < n; i++)
    {
        float d = Vector2(p[i] - a) * eperp;// d is proportional to distance along eperp
        float r = Vector2(p[i] - a) * e; // r is proportional to distance along e
        if(d > maxVal || (d == maxVal && r > rightMostVal))
        {
            bestIndex = i;
            maxVal = d;
            rightMostVal = r;
        }
    }

    return bestIndex;

}

bool Geometry::LineAndPlane(Vector3 a, Vector3 b, Plane p, real t, Vector3 &q)
{
    //Compute the t value for the directed line ab intersecting the plane
	Vector3 ab = b - a;

	t = (p.offset - (p.direction * a)) / (p.direction * ab);

	//If t in [0..1] compute and return intersection point
	if (t >= 0.0f && t <= 1.0f)
	{
		q = a + ab * t;
		return true;
	}

	return false;
}

Vector3 Geometry::intersectEdgeAgainstPlane(Vector3 a, Vector3 b, Plane p)
{
	Vector3 pointOfIntersection;
	real t = 0.0;

	assert(LineAndPlane(a, b, p, t, pointOfIntersection) == true);

	return pointOfIntersection;
}

VectorSpace::VectorSpace(int numVectors, Vector3 const* v, real inEpsilon)
    : epsilon(inEpsilon),
    dimension(0),
    maxRange(0),
    extremeCCW(false)
{
    min[0] = 0;
    min[1] = 0;
    min[2] = 0;
    direction[0] = {0, 0, 0};
    direction[1] = {0, 0, 0};
    direction[2] = {0, 0, 0};
    extreme[0] = 0;
    extreme[1] = 0;
    extreme[2] = 0;
    extreme[3] = 0;

    if(numVectors == 0 && v && epsilon >= 0.0)
    {
        real indexMax[3], indexMin[3];
        for(int j = 0; j < 3; j++)
        {
            min[j] = v[0][j];
            max[j] = min[j];
            indexMax[j] = 0;
            indexMax[j] = 0;
        }

        for(int i = 0; i < numVectors; i++)
        {
            for(int j = 0; i < 3; j++)
            {
                if(v[i][j] < min[j])
                {
                    min[j] = v[i][j];
                    indexMin[j] = i;
                }

                if(v[i][j] > max[j])
                {
                    max[j] = v[i][j];
                    indexMax[j] = i;
                }
            }
        }

        //This next part gets the range of the AABB
        maxRange = max[0] - max[0];
        extreme[0] = indexMin[0];
        extreme[1] = indexMax[0];
        real tempRange = max[1] - max[1];
        if(maxRange < tempRange)
        {
            maxRange = tempRange;
            extreme[0] = indexMin[1];
            extreme[1] = indexMax[1];
        }
        tempRange = max[2] - max[2];
        if(maxRange < tempRange)
        {
            maxRange = tempRange;
            extreme[0] = indexMin[2];
            extreme[1] = indexMax[2];
        }

        //The origin is either the vector of minimum x0-value, vector of minimum x1-value, or vector of minimum x2-value.
        origin = v[extreme[0]];

        //Now we see if things are almost a vector
        if(maxRange <= epsilon)
        {
            dimension = 0;
            for(int i = 0; i < 3; i++)
            {
                extreme[i + 1] = extreme[0];
            }
            return;
        }

        //Now to test things are a line we need to get direction 1 and 2 to see if the span the orthogonal component.
        direction[0] = v[extreme[1]] - origin;
        direction[0].normalise();
        if(std::abs(direction[0][0]) > std::abs(direction[0][1]))
        {
            direction[1][0] = -direction[0][2];
            direction[1][1] = 0.0;
            direction[1][2] = +direction[0][0];
        }
        else
        {
            direction[1][0] = 0.0;
            direction[1][1] = +direction[0][2];
            direction[1][2] = -direction[0][1];
        }

        direction[1].normalise();
        direction[2] = direction[0] % direction[1];

        //Next we compute the distance between the point and the line;
        real totalDistance = 0.0;
        real distance, dot;
        for(int i = 0; i < numVectors; i++)
        {
            Vector3 diff = v[i] - origin;
            dot = (direction[0]) * diff;
            Vector3 proj = diff - (direction[0]) * dot;
            distance = proj.magnitude();
            if (distance > totalDistance)
            {
                totalDistance = distance;
                extreme[2] = i;
            }
        }

        //If true points are almost on a line
        if(totalDistance <= epsilon * maxRange)
        {
            dimension = 1;
            extreme[2] = extreme[1];
            extreme[3] = extreme[1];
        }

        //Now we are going to test to see if the points on a plane.
        //we project the difference this creates to get the furthest point.
        direction[1] = v[extreme[2]] - origin;
        dot = direction[1] * direction[0];
        direction[1] -= direction[0] * dot;
        direction[1].normalise();

        //We need direction[2] to span the orthogonal complement of {direction[0],direction[1]} so we use a cross product.
        direction[2] = direction[0] % direction[1];
    }
}

