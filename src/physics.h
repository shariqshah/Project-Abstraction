#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#include "../include/glm/gtc/type_ptr.hpp"

#include "utilities.h"
#include "log.h"
#include "transform.h"

typedef btDiscreteDynamicsWorld PhysicsWorld;
typedef uint32_t                RBHandle;

class MotionState;

namespace Physics
{
	bool initialize(glm::vec3 gravity);
	
	void update(float deltaTime);
	void cleanup();
	void setGravity(glm::vec3 gravity);

	RBHandle createPlane(glm::vec3    normal,
						 MotionState* motionState,
						 float        mass,   
						 float        restitution);
	
	RBHandle createSphere(float        radius,         
						  MotionState* motionState,
						  float        mass,  
						  float        restitution);
	
	void setTransform(RBHandle body, glm::vec3  position, glm::quat  rotation);
	void getTransform(RBHandle body, glm::vec3* position, glm::quat* rotation);
	void setTransform(RBHandle body, glm::mat4 transformMat);
	void applyForce(RBHandle body, glm::vec3 force, glm::vec3 relPos);
	void removeRigidBody(RBHandle body);
	void setActivation(RBHandle body, bool activation);
	void setMass(RBHandle body, const float mass);
	
	const glm::vec3 getGravity();
}

class MotionState : public btMotionState
{
	std::weak_ptr<Transform> mTransformPtr;

public:
	MotionState(std::shared_ptr<Transform> transform)
	{
		mTransformPtr = transform;
	}

	virtual ~MotionState() {}

	virtual void getWorldTransform(btTransform& worldTrans) const
	{
		if(auto transform = mTransformPtr.lock())
		{
			glm::mat4 transformMat = transform->getTransformMat();
			worldTrans.setFromOpenGLMatrix(glm::value_ptr(transformMat));
		}
	}

	virtual void setWorldTransform(const btTransform& worldTrans)
	{
		if(auto transform = mTransformPtr.lock())
		{
			transform->setPosition(Utils::toGlm(worldTrans.getOrigin()));
			transform->setRotation(Utils::toGlm(worldTrans.getRotation()));
			transform->resetTransformFlag();
		}
	}
   
};


#endif
