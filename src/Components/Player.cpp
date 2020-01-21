#include "Player.h"

Player::Player(wind::real mRatio) :
	cam(wind::Vector3(0.0, 0.0, 0.0), 70.0, static_cast<wind::real>(mRatio), 0.01, 1000.0)
{
	wind::Box::body = new wind::RigidBody();

	cube = new Mesh();
	once = true;
	if (wind::Box::body == nullptr)
	{
		std::cerr << "Contact box primitive is null, check you assignment of new body" << std::endl;
	}
}

Player::~Player()
{
	delete cube;
}

void Player::setState(const wind::Vector3& pos, const wind::Vector3& half)
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

	body->setLinearDamping(0.95);
	body->setAngularDamping(0.8);
	body->clearAccumulator();
	body->setAcceleration(0.0, -9.81, 0.0);

	body->setCanSleep(false);
	body->setAwake();

	offsetVector = halfSize.componentProduct(halfSize);

	body->calculateDerivedData();
	wind::Box::calculateInternals();
}

void Player::loadMesh(const std::string& filePath)
{
	cube->addMesh(filePath, halfSize);
}

void Player::gravityOff()
{
	body->setAcceleration(0, 0, 0);
}

void Player::canSleep(const bool state)
{
	body->setCanSleep(state);
}

void Player::setSleep(const bool state)
{
	body->setAwake(state);
}

void Player::move(wind::Vector3 vel)
{
	body->setVelocity(vel);
	cam.move(vel);
}

void Player::rotate(const wind::Quaternion& rot)
{
	body->setRotation(wind::Vector3().rotate(rot));
	cam.rotate(rot);
}

void Player::update(wind::real duration)
{
	body->integrate(duration);
	cam.updateCamera(duration);
	calculateInternals();
}

void Player::changePosition(wind::Vector3 pos)
{
	body->setPosition(pos);
	cam.resetPosition(pos);
}

void Player::draw()
{
	cube->Draw();
}

wind::RigidBody* Player::getBody()
{
	return wind::Box::body;
}

Mesh* Player::getMesh()
{
	return cube;
}

wind::Camera Player::getCamera()
{
	return cam;
}