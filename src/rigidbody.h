#ifndef _rigidbody_H
#define _rigidbody_H

#include "physics.h"
#include "component.h"

class RigidBody : public Component
{
	RBHandle mHandle;

public:
	RigidBody(float radius       = 1.f,
			  float mass         = 1.f,
			  glm::vec3 position = glm::vec3(0),
			  glm::quat rotation = glm::quat(),
			  float restitution  = 0.5f);

	RigidBody(glm::vec3 normal,
			  float mass         = 0.f,
			  glm::vec3 position = glm::vec3(0),
			  glm::quat rotation = glm::quat(),
			  float restitution  = 0.5f);
	
	~RigidBody();

	void getTransformation(glm::vec3* position, glm::quat* rotation);

	const static  std::string sName;
	virtual const std::string getName();
};

#endif
