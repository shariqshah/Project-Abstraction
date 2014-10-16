#include "rigidbody.h"

const std::string RigidBody::sName = "RigidBody";

RigidBody::RigidBody(float     radius,
					 float     mass,
					 glm::vec3 position,
					 glm::quat rotation,
					 float     restitution)
{
	mType   = ComponentType::RIGIDBODY;
	mHandle = Physics::createSphere(radius, position, rotation, mass, restitution);
}

RigidBody::RigidBody(glm::vec3 normal,
					 float     mass,
					 glm::vec3 position,
					 glm::quat rotation,
					 float     restitution)
{
	mType   = ComponentType::RIGIDBODY;
	mHandle = Physics::createPlane(normal, position, rotation, mass, restitution);
}

RigidBody::~RigidBody()
{
	Physics::removeRigidBody(mHandle);
}

const std::string RigidBody::getName()
{
	return sName;
}

void RigidBody::getTransformation(glm::vec3* position, glm::quat* rotation)
{
	Physics::getTransform(mHandle, position, rotation);
}
