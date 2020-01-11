#include "Polygon.h"

using namespace wind;

Polygon::Polygon()
{
}

Polygon::Polygon(std::vector<Vector3> old)
{
    //vertices.clear();
    for(unsigned int j = 0; j < old.size(); j++)
    {
        vertices.push_back(old.at(j));
    }

}

Plane Polygon::createPlane()
{
    hyperPlane = computePlane(vertices[0], vertices[1], vertices[2]);
    return hyperPlane;
}

Vector3 Polygon::getVertex(unsigned int i)
{
    return vertices[i];
}
