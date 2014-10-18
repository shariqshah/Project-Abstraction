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

		static std::vector<RBHandle>          sFreeList;
		static std::vector<btRigidBody*>      sRigidBodies;
		static std::vector<btCollisionShape*> sCollisionShapes;
		static std::vector<CollisionShape*>   sShapes;
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
		sWorld->removeRigidBody(body);

		if(body && body->getMotionState())
			delete body->getMotionState();

		

		body == NULL ? Log::message("RB is NULL") : delete body;
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

		for(btCollisionShape* shape : sCollisionShapes)
			delete shape;

		for(CollisionShape* shape : sShapes)
		{
			if(shape)
			{
				shape->cleanup();
				delete shape;
			}
		}
		
		delete sWorld;
		delete sSolver;
		delete sDispatcher;
		delete sCollisionConfiguration;
		delete sBroadphase;

		sRigidBodies.clear();
		sCollisionShapes.clear();
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

	void addCollisionShape(CollisionShape* shape)
	{
		sShapes.push_back(shape);
	}

	RBHandle createRigidBody(CollisionShape* shape,
							 MotionState*    motionState,
							 float           mass,
							 float           restitution)
	{
		btVector3 inertia(0, 0, 0);
		if(mass != 0)
			shape->getCollisionShape()->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
														  motionState,
														  shape->getCollisionShape(),
														  inertia);
		btRigidBody* body = new btRigidBody(consInfo);
		body->setRestitution(restitution);
		sWorld->addRigidBody(body);
			
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
	
	void setTransform(RBHandle body, glm::mat4 transformMat)
	{
		btTransform transform;
		transform.setFromOpenGLMatrix(glm::value_ptr(transformMat));
		sRigidBodies[body]->setWorldTransform(transform);
	}

	void setActivation(RBHandle body, bool activation)
	{
		sRigidBodies[body]->activate(activation);
	}

	void setTransform(RBHandle body, glm::vec3 position, glm::quat rotation)
	{
		btTransform transform;
		transform.setOrigin(Utils::toBullet(position));
		transform.setRotation(Utils::toBullet(rotation));
		sRigidBodies[body]->setWorldTransform(transform);
	}

	void getTransform(RBHandle body, glm::vec3* position, glm::quat* rotation)
	{
		btTransform transform = sRigidBodies[body]->getWorldTransform();

		if(position)
			*position = Utils::toGlm(transform.getOrigin());
		if(rotation)
			*rotation = Utils::toGlm(transform.getRotation());
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

	void setKinematic(RBHandle body, bool kinematic)
	{
		if(kinematic)
		{
			sRigidBodies[body]->setFlags(sRigidBodies[body]->getFlags() |
										 btCollisionObject::CF_KINEMATIC_OBJECT);
			sRigidBodies[body]->setActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			sRigidBodies[body]->setFlags(~btCollisionObject::CF_KINEMATIC_OBJECT);
			sRigidBodies[body]->setActivationState(~DISABLE_DEACTIVATION);
		}
		
	}

	void setMass(RBHandle body, const float mass)
	{
		auto shape = sRigidBodies[body]->getCollisionShape();
		btVector3 inertia(0, 0, 0);
		if(mass != 0)
			shape->calculateLocalInertia(mass, inertia);

		sRigidBodies[body]->setMassProps(mass, inertia);
	}

	void applyForce(RBHandle body, glm::vec3 force, glm::vec3 relPos)
	{
		sRigidBodies[body]->applyForce(Utils::toBullet(force),
									   Utils::toBullet(relPos));
	}
}
