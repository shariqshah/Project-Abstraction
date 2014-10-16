#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/bullet/btBulletDynamicsCommon.h"

#include "utilities.h"
#include "log.h"

typedef btDiscreteDynamicsWorld PhysicsWorld;
typedef uint32_t                RBHandle;

namespace Physics
{
	bool initialize(glm::vec3 gravity);
	
	void update(float deltaTime);
	void cleanup();
	void setGravity(glm::vec3 gravity);

	RBHandle createSphere(float radius          = 1.f,
						  glm::vec3 position    = glm::vec3(0),
						  glm::quat rotation    = glm::quat(),
						  float     mass        = 1.f,
						  float     restitution = 0.5f);
	RBHandle createPlane(glm::vec3 normal,
						 glm::vec3 position    = glm::vec3(0),
						 glm::quat rotation    = glm::quat(),
						 float     mass        = 0.f,
						 float     restitution = 0.5f);

	void setTransform(RBHandle body, glm::vec3  position, glm::quat  rotation);
	void getTransform(RBHandle body, glm::vec3* position, glm::quat* rotation);
	void setTransform(RBHandle body, glm::mat4 transformMat);
	void removeRigidBody(RBHandle body);
	
	const glm::vec3 getGravity();
}

#endif
