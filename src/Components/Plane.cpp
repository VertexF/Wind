#include "Plane.h"

Wall::Wall() : mesh(std::make_unique<Mesh>())
{
	wind::Plane::body = new wind::RigidBody();
}

Wall::~Wall()
{
}

void Wall::setState(const wind::Vector3& pos, wind::Vector3 dir)
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
	//body->setAwake();

	wind::Plane::direction = dir.rotate(body->getOrientation());
	wind::Plane::offset = pos * wind::Plane::direction;
}

void Wall::loadMesh(const std::string& filePath)
{
	mesh->addMesh(filePath, wind::Vector3(1.0, 1.0, 1.0));
}

void Wall::initRotation(const wind::Vector3& axis, const wind::Vector3& pos, const wind::real& angle)
{
	wind::Plane::direction = wind::Vector3().rotate(angle, axis);
	wind::Plane::offset = pos * wind::Plane::direction;

	body->getOrientation().rotateByVector(wind::Plane::direction);
	body->setPosition(pos);
}

void Wall::update(wind::real duration)
{
	body->integrate(duration);
}

void Wall::draw()
{
	mesh->Draw();
}

wind::RigidBody* Wall::getBody()
{
	return body;
}

Mesh* Wall::getMesh()
{
	return mesh.get();
}
