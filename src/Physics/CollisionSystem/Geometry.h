#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <vector>

#include "../include/Core.h"
#include "collision_narrow.h"

class Geometry
{
    public:
        Geometry();

        inline wind::real triArea2D(wind::real x1, wind::real y1, wind::real x2, wind::real y2, wind::real x3, wind::real y3);

        /**
           In this function we are calculating the Barycentric coordinates and return that as a Vector3.

        */
        wind::Vector3 Barycentric(std::vector<wind::Vector3> Points, std::vector<wind::real> &coords);

        /**
            This function calculates the coordinates for v, u, w.
            With point Point[3] with respect to triangle of Point[0], Point[1], Point[2].
        */
        wind::Vector3 Barycentric(std::vector<wind::Vector3> Points, wind::real &v, wind::real &u, wind::real &w);
        /**
            This functions checks to see if the point p is in or outside triangle ABC
        */
        int testPointTriangle(wind::Vector3 p, wind::Vector3 a, wind::Vector3 b, wind::Vector3 c);

        /**
            What this function does is it computes a plane based of the triangle points using this formula:
                plane n = (B - A % C - A) <-- Which computes the vector
                signed distances form the origin  = plane n * A
        */
        wind::Plane computePlane(wind::Vector3 PointA, wind::Vector3 PointB, wind::Vector3 PointC);

        /**
            This just finds the point which is further from the edge.
        */
        int pointFarthestFromEdge(wind::Vector2 a, wind::Vector2 b, wind::Vector2 p[], int n);

};

#endif // GEOMETRY_H_INCLUDED
