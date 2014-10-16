#include "physics.h"

namespace Physics
{
	namespace
	{
		static PhysicsWorld* sWorld;
		static btVector3     sGravity;

		static btBroadphaseInterface*               sBroadphase;
		static btDefaultCollisionConfiguration*     sCollisionConfiguration;
		static btCollisionDispatcher*               sDispatcher;
		static btSequentialImpulseConstraintSolver* sSolver;

		static std::vector<RBHandle>    sFreeList;
		static std::vector<btRigidBody*> sRigidBodies;
	}

	bool initialize(glm::vec3 gravity)
	{
		sGravity = Utils::toBullet(gravity);

	    sBroadphase             = new btDbvtBroadphase();
		sCollisionConfiguration = new btDefaultCollisionConfiguration();
		sDispatcher             = new btCollisionDispatcher(sCollisionConfiguration);
		sSolver                 = new btSequentialImpulseConstraintSolver;

		sWorld                  = new btDiscreteDynamicsWorld(sDispatcher,
															  sBroadphase,
															  sSolver,
															  sCollisionConfiguration);
		sWorld->setGravity(sGravity);
	}

	void update(float deltaTime)
	{
		sWorld->stepSimulation(deltaTime);
	}

	void removeCollisionObject(btRigidBody *body)
    {
		//btRigidBody *body = btRigidBody::upcast(obj);

		if(body && body->getMotionState())
			delete body->getMotionState();

		sWorld->removeRigidBody(body);

		//body == NULL ? Log::message("RB is NULL") : delete body;
		//sWorld->removeCollisionObject(obj);
		// if(obj)
		// 	delete obj;
    }

	void cleanup()
	{
		//Remove collision objects
		for(int i = sWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject *obj = sWorld->getCollisionObjectArray()[i];
			removeCollisionObject(btRigidBody::upcast(obj));
		}
		
		delete sWorld;
		delete sSolver;
		delete sDispatcher;
		delete sCollisionConfiguration;
		delete sBroadphase;

		sRigidBodies.clear();
	}

	void setGravity(glm::vec3 gravity)
	{
		sGravity = Utils::toBullet(gravity);
		sWorld->setGravity(sGravity);
	}

	const glm::vec3 getGravity()
	{
		return Utils::toGlm(sGravity);
	}

	RBHandle initializeRigidBody(btVector3 inertia,
								 btCollisionShape* shape,
								 btDefaultMotionState* motionState,
								 float mass,
								 float restitution)
	{
		btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass,
																  motionState,
																  shape,
																  inertia);
		btRigidBody* body = new btRigidBody(constructionInfo);
		body->setGravity(btVector3(0, -9.8, 0));
		body->setRestitution(restitution);
		sWorld->addRigidBody(body);
			
		// delete shape;
		// delete motionState;
			
		RBHandle rbHandle = 0;
			
		if(!sFreeList.empty())
		{
			rbHandle = sFreeList.back();
			sFreeList.pop_back();

			if(sRigidBodies[rbHandle] != NULL)
				Log::warning("Overwriting Rigidbody!");
			
			sRigidBodies[rbHandle] = body;
		}
		else
		{
			sRigidBodies.push_back(body);
			rbHandle = (uint32_t) (sRigidBodies.size() - 1);
		}
			
		return rbHandle;
	}

	RBHandle createSphere(float     radius,
						  glm::vec3 position,
						  glm::quat rotation,
						  float     mass,
						  float     restitution)
	{
		btCollisionShape* shape = new btSphereShape(btScalar(radius));

		btVector3 inertia(0, 0, 0);
		if(mass != 0)
			shape->calculateLocalInertia(mass, inertia);

		btVector3 btPosition = Utils::toBullet(position);
		btQuaternion btRotation = Utils::toBullet(rotation);
		btTransform transform(btRotation, btPosition);
		btDefaultMotionState *motionState = new btDefaultMotionState(transform);

		return initializeRigidBody(inertia, shape, motionState, mass, restitution);
	}

	RBHandle createPlane(glm::vec3 normal,
						 glm::vec3 position,
						 glm::quat rotation,
						 float     mass,
						 float     restitution)
	{
		btCollisionShape* shape = new btStaticPlaneShape(Utils::toBullet(normal),
														 btScalar(1));

		btVector3 inertia(0, 0, 0);
		if(mass != 0)
			shape->calculateLocalInertia(mass, inertia);

		btVector3 btPosition = Utils::toBullet(position);
		btQuaternion btRotation = Utils::toBullet(rotation);
		btTransform transform(btRotation, btPosition);
		btDefaultMotionState *motionState = new btDefaultMotionState(transform);

		return initializeRigidBody(inertia, shape, motionState, mass, restitution);
	}

	void setTransform(RBHandle body, glm::mat4 transformMat)
	{
		btTransform transform;
		transform.setFromOpenGLMatrix(glm::value_ptr(transformMat));

		sRigidBodies[body]->setWorldTransform(transform);
	}

	void getTransform(RBHandle body, glm::vec3* position, glm::quat* rotation)
	{
		btTransform transform;
		if(sRigidBodies[body])
		{
			sRigidBodies[body]->getMotionState()->getWorldTransform(transform);

			*position = Utils::toGlm(transform.getOrigin());
			*rotation = Utils::toGlm(transform.getRotation());
		}
		else
			Log::message("Not Found!");
		
	}

	void removeRigidBody(RBHandle body)
	{
		if(sRigidBodies[body])
		{
			removeCollisionObject(sRigidBodies[body]);
			sRigidBodies[body] = NULL;
			sFreeList.push_back(body);
		}
		else
			Log::warning("RigidBody " + std::to_string(body) + "does not exist so not removed");
	}
}
