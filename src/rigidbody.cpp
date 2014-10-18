#include "rigidbody.h"

const std::string RigidBody::sName = "RigidBody";

RigidBody::RigidBody(std::shared_ptr<Transform> transform,
					 glm::vec3 normal,
					 float     mass,
					 float     restitution)
{
	mType   = ComponentType::RIGIDBODY;
	mMotionState = new MotionState(transform);
	mHandle = Physics::createPlane(normal, mMotionState, mass, restitution);
}

RigidBody::RigidBody(std::shared_ptr<Transform> transform,
					 float     radius,
					 float     mass,
					 float     restitution)
{
	mType   = ComponentType::RIGIDBODY;
	mMotionState = new MotionState(transform);
	mHandle = Physics::createSphere(radius, mMotionState, mass, restitution);
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

void RigidBody::applyForce(const glm::vec3& force, const glm::vec3& relPos)
{
	Physics::applyForce(mHandle, force, relPos);
}

void RigidBody::setTransform(const glm::mat4 transformationMatrix)
{
	Physics::setTransform(mHandle, transformationMatrix);
}

void RigidBody::setMass(float mass)
{
	Physics::setMass(mHandle, mass);
}	

void RigidBody::setActivation(bool activation)
{
	Physics::setActivation(mHandle, activation);
}
