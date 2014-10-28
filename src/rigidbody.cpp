#include "rigidbody.h"

const std::string RigidBody::sName = "RigidBody";

RigidBody::RigidBody(std::shared_ptr<Transform> transform,
					 CollisionShape* shape,
					 float mass,
					 float restitution)
{
	mType   = ComponentType::RIGIDBODY;
	mMotionState = new MotionState(transform);
	//mShape  = shape;
	mHandle = Physics::createRigidBody(shape, mMotionState, mass, restitution);
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

void RigidBody::setKinematic(bool kinematic)
{
	Physics::setKinematic(mHandle, kinematic);
}
