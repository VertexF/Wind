#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <vector>

#include "Core.h"
#include "../CollisionSystem/collision_narrow.h"

namespace wind
{
    class Geometry
    {
        public:
            Geometry();

            inline real triArea2D(real x1, real y1, real x2, real y2, real x3, real y3);

            /**
               In this function we are calculating the Barycentric coordinates and return that as a Vector3.

            */
            Vector3 Barycentric(std::vector<Vector3> Points, std::vector<real> &coords);

            /**
                This function calculates the coordinates for v, u, w.
                With point Point[3] with respect to triangle of Point[0], Point[1], Point[2].
            */
            void Barycentric(std::vector<Vector3> Points, real &v, real &u, real &w);
            /**
                This functions checks to see if the point p is in or outside triangle ABC
            */
            int testPointTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c);

            /**
                What this function does is it computes a plane based of the triangle points using this formula:
                    plane n = (B - A % C - A) <-- Which computes the vector
                    signed distances form the origin  = plane n * A
            */
            Plane computePlane(Vector3 PointA, Vector3 PointB, Vector3 PointC);

            /**
                This just finds the point which is further from the edge.
            */
            int pointFarthestFromEdge(Vector2 a, Vector2 b, Vector2 p[], int n);

            /**
                This is the intersection test for a plane and a line segment.
            */
			static bool LineAndPlane(Vector3 a, Vector3 b, Plane p, real t, Vector3 &q);

            /**
                This function additionally returns the point of intersection
            */
            Vector3 intersectEdgeAgainstPlane(Vector3 a, Vector3 b, Plane p);
    };
    /**
        TODO: Complete the algorithm
        Find the best place for the algorithm to go.
    */
    /**
        This class is a foundational class to convex hull generation.
        What this class actual does it is finds extreme points based of points passed into the class.
        This creates a line, square or a AABB based on the dimensions found.
        These are created and returned at each stage of the algorithm going through and returning the extremes and origin of that
        dimensions.
        This creates a space where all points are found and within is the origin which sets up the direction the space is facing.
    */
    class VectorSpace
    {
        public:
            VectorSpace(int numVectors, Vector3 const* v, real inEpsilon);
        private:
            /** This is the fault tolerance if points are less than the epsilon they are either on the same plane, line or point as each other*/
            real epsilon;
            /** Simply holds the dimensions of the vector space.*/
            int dimension;

            /** This is how we are storing the AABB itself using extreme of min and max values, for now.*/
            real max[3];
            real min[3];
            /**  The max range is the size of the sides with the largest length.*/
            real maxRange;

            Vector3 origin;
            Vector3 direction[3];

            /** These are the indices that define the vector space maximums depending on the dimensions*/
            /** The values extreme[0] and extreme[1] are the indices for the points that define the largest extent in one of the coordinate axis directions*/
            int extreme[4];
            /** The tetrahedron formed by the extreme[0] through 3 direction is defined by this bool.*/
            bool extremeCCW;
    };

}

#endif // GEOMETRY_H_INCLUDED
