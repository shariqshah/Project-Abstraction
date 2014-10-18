#ifndef _rigidbody_H
#define _rigidbody_H

#include "physics.h"
#include "component.h"

class RigidBody : public Component
{
	RBHandle     mHandle;
	MotionState *mMotionState;
public:
	RigidBody(std::shared_ptr<Transform> transform,
			  float radius       = 1.f,
			  float mass         = 1.f,
			  float restitution  = 1.f);

	RigidBody(std::shared_ptr<Transform> transform,
			  glm::vec3 normal,
			  float mass         = 0.f,
			  float restitution  = 0.5f);
	
	~RigidBody();

	void getTransformation(glm::vec3* position, glm::quat* rotation);
	void setTransform(const glm::mat4 transformationMatrix);
	void applyForce(const glm::vec3& force, const glm::vec3& relPos = glm::vec3(0));
	void setMass(float mass);
	void setActivation(bool activation);
	
	const static  std::string sName;
	virtual const std::string getName();
};

#endif
