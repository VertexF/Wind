#ifndef POLYGON_H_INCLUDED
#define POLYGON_H_INCLUDED

#include <vector>

#include "Core.h"
#include "Geometry.h"
#include "../CollisionSystem/collision_narrow.h"

namespace wind
{
    /**
        This class holds the data for the polygon.

        Note: This only works with 3 points, if you are loading in four or more the maths is impossible.
    */
    class Polygon : public Geometry
    {
        public:
            Polygon();

            Polygon(std::vector<Vector3> old);

            /**
                This function sets up the plane based of the face of the polygon.
                NOTE: Only currently uses the first three elements so this maybe bad later on.
            */
            Plane createPlane();

            /**
                For building a BSP I need to know the number of vertices
                This is very redudent but if I am going to do this right I need to return it
            */
            inline unsigned int numberOfVertices() { return vertices.size(); }

            /**
                This function get the current vertex based of the index value passed in
            */
            Vector3 getVertex(unsigned int i);

            std::vector<Vector3> vertices;

            //This hold a hyperplane based off the vertices.
            Plane hyperPlane;
    };
}

#endif // POLYGON_H_INCLUDED
