#ifndef LEVELGEOMETRY_H_INCLUDED
#define LEVELGEOMETRY_H_INCLUDED

#include "../Physics/include/Body.h"
#include "../Physics/include/Polygon.h"
#include "CompRegistration.h"

class LevelGeometry
{
    public:
        LevelGeometry();
        void loadMesh(const std::string filePath, wind::Vector3 size = wind::Vector3(1.0, 1.0, 1.0));
        void setState(const wind::Vector3& pos);
        void update(wind::real duration);

        void draw();
        wind::RigidBody* getBody();
        Mesh* getMesh();

    private:
        std::vector<wind::Polygon *> polygons;

        Mesh* mesh;
        wind::RigidBody* body;

        bool once;
};

#endif // LEVELGEOMETRY_H_INCLUDED
