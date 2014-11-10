#include "physics.h"
#include "camera.h"

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

		static std::vector<CRigidBody>          sFreeList;
		static std::vector<btRigidBody*>      sRigidBodies;
		static std::vector<btCollisionShape*> sCollisionShapes;
		static std::vector<CollisionShape*>   sShapes;

		static DebugDrawer* sDebugDrawer;
		static bool         sEnableDebugDraw;
		static DBG_Mode     sCurrentDebugMode;
	}

	void initialize(glm::vec3 gravity)
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

		sCurrentDebugMode = DBG_Mode::AABB;
		sDebugDrawer = new Physics::DebugDrawer();
		sDebugDrawer->setDebugMode((int)sCurrentDebugMode);
		sWorld->setDebugDrawer(sDebugDrawer);
		sEnableDebugDraw = false;
	}

	void update(float deltaTime)
	{
		sWorld->stepSimulation(deltaTime);
	}

	void draw(CTransform* viewerTransform, CCamera* viewerCamera)
	{
		if(sEnableDebugDraw && viewerTransform != NULL && viewerCamera != NULL)
		{
			Mat4 camTranMat = viewerTransform->transMat;
			Mat4 camProjMat;
		
			h3dGetCameraProjMat(viewerCamera->node,
								glm::value_ptr(camProjMat));

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadMatrixf(glm::value_ptr(glm::inverse(camTranMat)));
		
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadMatrixf(glm::value_ptr(camProjMat));

			glMatrixMode(GL_MODELVIEW);
		
			sWorld->debugDrawWorld();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glPopAttrib();
		}
		
	}

	void enableDebugDraw(bool enable)
	{
		sEnableDebugDraw = enable;
	}

	void setDebugMode(DBG_Mode debugMode)
	{
		sCurrentDebugMode = debugMode;
		sWorld->getDebugDrawer()->setDebugMode((int)sCurrentDebugMode);
	}

	void nextDebugMode()
	{
		DBG_Mode tempMode;
		
		switch(sCurrentDebugMode)
		{
		case DBG_Mode::NONE:
			tempMode = DBG_Mode::AABB;
			break;
		case DBG_Mode::AABB:
			tempMode = DBG_Mode::WIREFRAME;
			break;
		case DBG_Mode::WIREFRAME:
			tempMode = DBG_Mode::NORMALS;
			break;
		case DBG_Mode::NORMALS:
			tempMode = DBG_Mode::CONTACTS;
			break;
		case DBG_Mode::CONTACTS:
			tempMode = DBG_Mode::CONSTRAINTS;
			break;
		case DBG_Mode::CONSTRAINTS:
			tempMode = DBG_Mode::CONSTRAINT_LIMIT;
			break;
		case DBG_Mode::CONSTRAINT_LIMIT:
			tempMode = DBG_Mode::MAX;
			break;
		case DBG_Mode::MAX:
			tempMode = DBG_Mode::NONE;
			break;
		};

		setDebugMode(tempMode);
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
		delete sDebugDrawer;
		
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

	namespace RigidBody
	{
		CRigidBody create(CollisionShape* shape,
						  btMotionState*  motionState,
						  float           mass,
						  float           restitution)
		{
			btVector3 inertia(0, 0, 0);
			if(mass != 0)
				shape->getCollisionShape()->calculateLocalInertia(mass, inertia);

			//MotionState* motionState = new MotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
															  motionState,
															  shape->getCollisionShape(),
															  inertia);
			btRigidBody* body = new btRigidBody(consInfo);
			body->setRestitution(restitution);
			sWorld->addRigidBody(body);
			
			CRigidBody rbHandle = -1;
			
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
				rbHandle = (int32_t) (sRigidBodies.size() - 1);
			}
			
			return rbHandle;
		}
	
		void setTransform(CRigidBody body, glm::mat4 transformMat)
		{
			btTransform transform;
			transform.setFromOpenGLMatrix(glm::value_ptr(transformMat));
			sRigidBodies[body]->setWorldTransform(transform);
		}

		void setActivation(CRigidBody body, bool activation)
		{
			sRigidBodies[body]->activate(activation);
		}

		void setTransform(CRigidBody body, glm::vec3 position, glm::quat rotation)
		{
			btTransform transform;
			transform.setOrigin(Utils::toBullet(position));
			transform.setRotation(Utils::toBullet(rotation));
			sRigidBodies[body]->setWorldTransform(transform);
		}

		void getTransform(CRigidBody body, glm::vec3* position, glm::quat* rotation)
		{
			btTransform transform = sRigidBodies[body]->getWorldTransform();

			if(position)
				*position = Utils::toGlm(transform.getOrigin());
			if(rotation)
				*rotation = Utils::toGlm(transform.getRotation());
		}

		void remove(CRigidBody body)
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

		void setKinematic(CRigidBody body, bool kinematic)
		{
			btRigidBody* temp = sRigidBodies[body];
			sWorld->removeRigidBody(temp);

			if(kinematic)
			{
				temp->setFlags(sRigidBodies[body]->getFlags() |
							   btCollisionObject::CF_KINEMATIC_OBJECT |
							   btCollisionObject::CF_NO_CONTACT_RESPONSE);
				temp->setActivationState(DISABLE_DEACTIVATION);
			}
			else
			{
				//sRigidBodies[body]->setFlags(~btCollisionObject::CF_KINEMATIC_OBJECT);
				temp->setFlags(sRigidBodies[body]->getFlags() |
							   btCollisionObject::CF_STATIC_OBJECT);
				temp->setActivationState(WANTS_DEACTIVATION);
				//sRigidBodies[body]->activate(true);
			}

			sWorld->addRigidBody(temp);
			// if(kinematic)
			// {
			// 	sRigidBodies[body]->setFlags(sRigidBodies[body]->getFlags() |
			// 								 btCollisionObject::CF_KINEMATIC_OBJECT |
			// 								 btCollisionObject::CF_NO_CONTACT_RESPONSE);
			// 	sRigidBodies[body]->setActivationState(DISABLE_DEACTIVATION);
			// }
			// else
			// {
			// 	//sRigidBodies[body]->setFlags(~btCollisionObject::CF_KINEMATIC_OBJECT);
			// 	sRigidBodies[body]->setFlags(sRigidBodies[body]->getFlags() |
			// 								 ~(btCollisionObject::CF_KINEMATIC_OBJECT) |
			// 								 btCollisionObject::CF_STATIC_OBJECT);
			// 	sRigidBodies[body]->setActivationState(WANTS_DEACTIVATION);
			// 	//sRigidBodies[body]->activate(true);
			// }
		
		}

		void setMass(CRigidBody body, const float mass)
		{
			auto shape = sRigidBodies[body]->getCollisionShape();
			btVector3 inertia(0, 0, 0);
			if(mass != 0)
				shape->calculateLocalInertia(mass, inertia);

			sRigidBodies[body]->setMassProps(mass, inertia);
		}

		void applyForce(CRigidBody body, glm::vec3 force, glm::vec3 relPos)
		{
			sRigidBodies[body]->applyForce(Utils::toBullet(force),
										   Utils::toBullet(relPos));
		}
	}
}
