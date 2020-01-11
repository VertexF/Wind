#include "Block.h"

Block::Block()
{
	wind::Box::body = new wind::RigidBody();
	cube = new Mesh();
	if(wind::Box::body == nullptr)
	{
		std::cerr << "Contact box primitive is null, check you assignment of new body" << std::endl;
	}
}

Block::~Block()
{
	delete cube;
}

void Block::setState(const wind::Vector3& pos, const wind::Vector3& half)
{
	body->setPosition(pos);
	body->setOrientation(0.0, 0.0, 0.0, 0.0);
	body->setVelocity(0.0, 0.0, 0.0);
	body->setRotation(0.0, 0.0, 0.0);
	halfSize = half;

	wind::real mass = halfSize.x * halfSize.z * halfSize.y * 0.8;
	body->setInverseMass(mass);

	wind::Matrix3 tensor;
    tensor.setBlockInertiaTensor(halfSize, mass);
    body->setInertiaTensor(tensor);

    body->setAcceleration(0.0, -9.81, 0.0);

	body->setLinearDamping(0.95);
	body->setAngularDamping(0.8);
	body->clearAccumulator();

	body->setCanSleep(true);
	body->setAwake();

	body->calculateDerivedData();
	wind::Box::calculateInternals();
}

void Block::loadMesh(const std::string& filePath)
{
	cube->addMesh(filePath, halfSize);
}

void Block::gravityOff()
{
	body->setAcceleration(0, 0, 0);
}

void Block::canSleep(const bool state)
{
	body->setCanSleep(state);
}

void Block::setSleep(const bool state)
{
	body->setAwake(state);
}

void Block::update(wind::real duration)
{
	body->integrate(duration);
	calculateInternals();
}

void Block::changePosition(wind::Vector3 pos)
{
    body->setPosition(pos);
}

void Block::draw()
{
	cube->Draw();
}

wind::RigidBody* Block::getBody()
{
	return wind::Box::body;
}

Mesh* Block::getMesh()
{
	return cube;
}
