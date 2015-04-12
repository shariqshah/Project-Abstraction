#include "rigidbody.h"
#include "gameobject.h"
#include "scriptengine.h"
#include "utilities.h"
#include "log.h"
#include "passert.h"
#include "physics.h"
#include "transform.h"
#include "collisionshapes.h"

#include "../include/bullet/btBulletDynamicsCommon.h"

namespace RigidBody
{
	namespace
	{
		std::vector<CRigidBody>      freeList;
		std::vector<btRigidBody*>    rigidBodies;

		void removeCollisionObject(btRigidBody *body)
		{
			btDiscreteDynamicsWorld* world = Physics::getWorld();
			world->removeRigidBody(body);
			if(body && body->getMotionState()) delete body->getMotionState();
			body == NULL ? Log::message("RB is NULL") : delete body;
		}
	}
	
	CRigidBody create(GameObject*     gameObject,
					  CollisionShape* shape,
					  btMotionState*  motionState,
					  float           mass,
					  float           restitution)
	{
		btDiscreteDynamicsWorld* world = Physics::getWorld();
		btVector3 inertia(0, 0, 0);
		if(mass != 0) shape->getCollisionShape()->calculateLocalInertia(mass, inertia);
		btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
														  motionState,
														  shape->getCollisionShape(),
														  inertia);
		btRigidBody* body = new btRigidBody(consInfo);
		body->setRestitution(restitution);
		intptr_t temp = (intptr_t)gameObject->node;
		body->setUserPointer((void*)temp);
		world->addRigidBody(body);
		CRigidBody rbHandle = -1;
			
		if(!freeList.empty())
		{
			rbHandle = freeList.back();
			freeList.pop_back();

			if(rigidBodies[rbHandle] != NULL)
				Log::warning("Overwriting Rigidbody!");
			
			rigidBodies[rbHandle] = body;
		}
		else
		{
			rigidBodies.push_back(body);
			rbHandle = (int32_t) (rigidBodies.size() - 1);
		}
		CTransform* transform = GO::getTransform(gameObject);
		setTransform(rbHandle, transform->transMat);
		return rbHandle;
	}
	
	void setTransform(CRigidBody body, Mat4 transformMat)
	{
		btTransform transform;
		transform.setFromOpenGLMatrix(glm::value_ptr(transformMat));
		rigidBodies[body]->setWorldTransform(transform);
	}

	void setActivation(CRigidBody body, bool activation)
	{
		rigidBodies[body]->activate(activation);
	}

	void setTransform(CRigidBody body, Vec3 position, Quat rotation)
	{
		btTransform transform;
		transform.setOrigin(Utils::toBullet(position));
		transform.setRotation(Utils::toBullet(rotation));
		rigidBodies[body]->setWorldTransform(transform);
	}

	void getTransform(CRigidBody body, glm::vec3* position, glm::quat* rotation)
	{
		btTransform transform = rigidBodies[body]->getWorldTransform();
		if(position)
			*position = Utils::toGlm(transform.getOrigin());
		if(rotation)
			*rotation = Utils::toGlm(transform.getRotation());
	}

	void remove(CRigidBody body)
	{
		if(rigidBodies[body])
		{
			removeCollisionObject(rigidBodies[body]);
			rigidBodies[body] = NULL;
			freeList.push_back(body);
		}
		else
		{
			Log::warning("RigidBody " + std::to_string(body) + "does not exist so not removed");
		}
	}

	void setKinematic(CRigidBody body, bool kinematic)
	{
		btRigidBody* temp = rigidBodies[body];
		btDiscreteDynamicsWorld* world = Physics::getWorld();
		world->removeRigidBody(temp);

		if(kinematic)
		{
			temp->setFlags(rigidBodies[body]->getFlags() |
						   btCollisionObject::CF_KINEMATIC_OBJECT |
						   btCollisionObject::CF_NO_CONTACT_RESPONSE);
			temp->setActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			//rigidBodies[body]->setFlags(~btCollisionObject::CF_KINEMATIC_OBJECT);
			temp->setFlags(rigidBodies[body]->getFlags() |
						   btCollisionObject::CF_STATIC_OBJECT);
			temp->setActivationState(WANTS_DEACTIVATION);
			//rigidBodies[body]->activate(true);
		}

		world->addRigidBody(temp);
		// if(kinematic)
		// {
		// 	rigidBodies[body]->setFlags(rigidBodies[body]->getFlags() |
		// 								 btCollisionObject::CF_KINEMATIC_OBJECT |
		// 								 btCollisionObject::CF_NO_CONTACT_RESPONSE);
		// 	rigidBodies[body]->setActivationState(DISABLE_DEACTIVATION);
		// }
		// else
		// {
		// 	//rigidBodies[body]->setFlags(~btCollisionObject::CF_KINEMATIC_OBJECT);
		// 	rigidBodies[body]->setFlags(rigidBodies[body]->getFlags() |
		// 								 ~(btCollisionObject::CF_KINEMATIC_OBJECT) |
		// 								 btCollisionObject::CF_STATIC_OBJECT);
		// 	rigidBodies[body]->setActivationState(WANTS_DEACTIVATION);
		// 	//rigidBodies[body]->activate(true);
		// }
		
	}

	void setMass(CRigidBody body, float mass)
	{
		btCollisionShape* shape = rigidBodies[body]->getCollisionShape();
		btVector3 inertia(0, 0, 0);
		if(mass <  0.f) mass = 0.f;
		if(mass != 0.f) shape->calculateLocalInertia(mass, inertia);
		rigidBodies[body]->setMassProps(mass, inertia);
	}

	float getMass(CRigidBody body)
	{
		float mass = rigidBodies[body]->getInvMass();
		if(mass != 0.f) mass = 1.f / mass;
		return mass;
	}

	float getRestitution(CRigidBody body)
	{
		return rigidBodies[body]->getRestitution();
	}
		
	void setRestitution(CRigidBody body, float restitution)
	{
		if(restitution < 0.f) restitution *= -1.f;
		rigidBodies[body]->setRestitution(restitution);
	}
		
	float getFriction(CRigidBody body)
	{
		return rigidBodies[body]->getFriction();
	}
		
	void setFriction(CRigidBody body, float friction)
	{
		if(friction < 0.f) friction *= -1.f;
		rigidBodies[body]->setFriction(friction);
	}

	void applyForce(CRigidBody body, Vec3 force, Vec3 relPos)
	{
		rigidBodies[body]->applyForce(Utils::toBullet(force),
									  Utils::toBullet(relPos));
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		engine->SetDefaultNamespace("Rigidbody");
		int rc = engine->RegisterGlobalFunction("void setKinematic(int, bool)",
												asFUNCTION(setKinematic),
												asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("float getMass(int)",
											asFUNCTION(getMass),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setMass(int, float)",
											asFUNCTION(setMass),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("float getRestitution(int)",
											asFUNCTION(getRestitution),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setRestitution(int, float)",
											asFUNCTION(setRestitution),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("float getFriction(int)",
											asFUNCTION(getFriction),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setFriction(int, float)",
											asFUNCTION(setFriction),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void applyForce(int, const Vec3, const Vec3 = Vec3(0.f))",
											asFUNCTION(applyForce),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
	}

	void setCollisionShape(CRigidBody body, CollisionShape* shape)
	{
		PA_ASSERT(shape);
		rigidBodies[body]->setCollisionShape(shape->getCollisionShape());
	}

	const char* getCollisionShapeName(CRigidBody body)
	{
		return rigidBodies[body]->getCollisionShape()->getName();
	}

	void cleanup()
	{
		btDiscreteDynamicsWorld* world = Physics::getWorld();
		//Remove collision objects
		for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject *obj = world->getCollisionObjectArray()[i];
			removeCollisionObject(btRigidBody::upcast(obj));
		}
		rigidBodies.clear();
	} 
}
