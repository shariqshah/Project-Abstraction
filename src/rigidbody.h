#ifndef _rigidbody_H
#define _rigidbody_H

#include "datatypes.h"
#include "mathdefs.h"

class  CollisionShape;
struct GameObject;
class  btMotionState;

namespace RigidBody
{
	CRigidBody create(GameObject*     gameObject,
					  CollisionShape* shape,
					  btMotionState*  motionState,
					  float           mass,
					  float           restitution);
	void  setTransform(CRigidBody body, Vec3  position, Quat  rotation);
	void  getTransform(CRigidBody body, Vec3* position, Quat* rotation);
	void  setTransform(CRigidBody body, Mat4 transformMat);
	void  applyForce(CRigidBody body, Vec3 force, Vec3 relPos = Vec3(0.f));
	void  remove(CRigidBody body);
	void  setActivation(CRigidBody body, bool activation);
	void  setKinematic(CRigidBody body, bool kinematic);
	void  generateBindings();
	void  setCollisionShape(CRigidBody body, CollisionShape* shape);
	void  setMass(CRigidBody body, float mass);
	float getMass(CRigidBody body);
	float getRestitution(CRigidBody body);
	void  setRestitution(CRigidBody body, float restitution);
	float getFriction(CRigidBody body);
	void  setFriction(CRigidBody body, float friction);
	const char* getCollisionShapeName(CRigidBody body);
	void  cleanup();
}

#endif
