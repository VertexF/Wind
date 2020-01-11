#include "LevelGeometry.h"

LevelGeometry::LevelGeometry()
{
    mesh = new Mesh();
    body = new wind::RigidBody();

    once = true;
}

void LevelGeometry::loadMesh(const std::string filePath, wind::Vector3 size)
{
    mesh->addMesh(filePath, size, polygons);
}

void LevelGeometry::setState(const wind::Vector3& pos)
{
	body->setPosition(pos);
	body->setOrientation(0.0, 0.0, 0.0, 0.0);
	body->setVelocity(0.0, 0.0, 0.0);
	body->setRotation(0.0, 0.0, 0.0);
	wind::Vector3 halfSize = wind::Vector3(1.0, 1.0, 1.0);

    wind::real mass3 = halfSize.x * halfSize.z * halfSize.y * 0.8;
	body->setInverseMass(mass3);

	wind::Matrix3 tensor;
    tensor.setBlockInertiaTensor(halfSize, mass3);
    body->setInertiaTensor(tensor);

	body->setLinearDamping(0.95);
	body->setAngularDamping(0.8);
	body->clearAccumulator();

	body->setCanSleep(false);
	body->setAwake();
}

void LevelGeometry::update(wind::real duration)
{
    if(once)
    {
        body->integrate(duration);

        for(int i = 0; i < polygons.size(); i++)
        {
            polygons[i]->vertices[0] = body->getTransform() * polygons[i]->vertices[0];
            polygons[i]->vertices[1] = body->getTransform() * polygons[i]->vertices[1];
            polygons[i]->vertices[2] = body->getTransform() * polygons[i]->vertices[2];
        }

        once = false;
    }
}

void LevelGeometry::draw()
{
	mesh->Draw();
}

wind::RigidBody* LevelGeometry::getBody()
{
	return body;
}

Mesh* LevelGeometry::getMesh()
{
	return mesh;
}
