#include "Geometry.h"

Geometry::Geometry()
{

}

inline wind::real Geometry::triArea2D(wind::real x1, wind::real y1, wind::real x2, wind::real y2, wind::real x3, wind::real y3)
{
    return(((x1 - x2) * (y2 - y3)) - ((x2 - x3) * (y1 - y2)));
}

wind::Vector3 Geometry::Barycentric(std::vector<wind::Vector3> Points, std::vector<wind::real> &coords)
{
    for(unsigned int i = 0; i < Points.size(); i++)
    {
        Points.at(i).makePoint();
    }

    wind::Vector3 vec1(Points.at(1) - Points.at(0));
    wind::Vector3 vec2(Points.at(2) - Points.at(0));
    wind::Vector3 vec3(Points.at(3) - Points.at(0));

    wind::real d00 = vec1 * vec1;
    wind::real d01 = vec1 * vec2;
    wind::real d11 = vec2 * vec2;
    wind::real d20 = vec3 * vec1;
    wind::real d21 = vec3 * vec2;

    wind::real demon = d00 * d11 - d01 * d01;

    wind::real v = ((d11 * d20 - d01 * d21) / demon);
    wind::real w = ((d00 * d21 - d01 * d20) / demon);

    wind::Vector3 result(v, w, (1.0 - v - w));

    return result;
}

void Geometry::Barycentric(std::vector<wind::Vector3> Points, wind::real &v, wind::real &u, wind::real &w)
{
    for(unsigned int i = 0; i < Points.size(); i++)
    {
        Points.at(i).makePoint();
    }

    //First we need to unnormalised the triangle normal.
    wind::Vector3 m = wind::Vector3(Points[1] - Points[0]) % wind::Vector3(Points[2] - Points[0]);
    // Nominators and one-over-denominator for v and u ratios.
    wind::real nu, nv, ood;
    //Absolute components for determining projection plane.
    wind::real x = std::abs(m.x);
    wind::real y = std::abs(m.y);
    wind::real z = std::abs(m.z);

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

int Geometry::testPointTriangle(wind::Vector3 p, wind::Vector3 a, wind::Vector3 b, wind::Vector3 c)
{
    wind::real u, v, w;
    std::vector<wind::Vector3> jack;
    jack.at(0) = a;
    jack.at(1) = b;
    jack.at(2) = c;
    jack.at(3) = p;
    Barycentric(jack, u, v, w);
    return(v >= 0.0 && w >= 0.0 && (v + w) <= 1.0);
}

wind::Plane Geometry::computePlane(wind::Vector3 PointA, wind::Vector3 PointB, wind::Vector3 PointC)
{
    wind::Plane p;
    p.direction = wind::Vector3(PointB - PointA) % wind::Vector3(PointC - PointA);
    p.offset = p.direction * PointA;
    return p;
}

int Geometry::pointFarthestFromEdge(wind::Vector2 a, wind::Vector2 b, wind::Vector2 p[], int n)
{
    // Create edge vector and vector (counterclockwise) perpendicular to it
    wind::Vector2 e = b - a;
    wind::Vector2 eperp(-e.y, e.x);
    // Track index, ‘distance’ and ‘rightmostness’ of currently best point
    int bestIndex = -1;
    float maxVal = std::numeric_limits<float>::min(), rightMostVal = std::numeric_limits<float>::min();
    // Test all points to find the one farthest from edge ab on the left side
    for(int i = 1; i < n; i++)
    {
        float d = wind::Vector2(p[i] - a) * eperp;// d is proportional to distance along eperp
        float r = wind::Vector2(p[i] - a) * e; // r is proportional to distance along e
        if(d > maxVal || (d == maxVal && r > rightMostVal))
        {
            bestIndex = i;
            maxVal = d;
            rightMostVal = r;
        }
    }

    return bestIndex;

}
