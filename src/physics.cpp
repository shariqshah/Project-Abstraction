#include "physics.h"
#include "camera.h"
#include "transform.h"
#include "scriptengine.h"
#include "gameobject.h"

namespace Physics
{
	namespace
	{
		PhysicsWorld* sWorld;
		btVector3     sGravity;

		btBroadphaseInterface*               sBroadphase;
		btDefaultCollisionConfiguration*     sCollisionConfiguration;
		btCollisionDispatcher*               sDispatcher;
		btSequentialImpulseConstraintSolver* sSolver;

		std::vector<CRigidBody>          sFreeList;
		std::vector<btRigidBody*>        sRigidBodies;
		std::vector<CollisionShape*>     sShapes;

		DebugDrawer* sDebugDrawer;
		bool         sEnableDebugDraw;
		DBG_Mode     sCurrentDebugMode;
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

		// Process collisions
		int numManifolds = sWorld->getDispatcher()->getNumManifolds();
		for (int i=0;i<numManifolds;i++)
		{
			btPersistentManifold* contactManifold =  sWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();

			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance()<0.f)
				{
					const btVector3& ptA       = pt.getPositionWorldOnA();
					const btVector3& ptB       = pt.getPositionWorldOnB();
					const btVector3& normalOnB = pt.m_normalWorldOnB;

					GameObject* gameObjectA = (GameObject*)obA->getUserPointer();
					GameObject* gameObjectB = (GameObject*)obB->getUserPointer();
					
					CollisionData collisionData;
					collisionData.collidingObj = gameObjectB;
					collisionData.normal       = Utils::toGlm(normalOnB);
					collisionData.worldPosA    = Utils::toGlm(ptA);
					collisionData.worldPosB    = Utils::toGlm(ptB);
 
					GO::processCollision(gameObjectA, collisionData);

					collisionData.collidingObj = gameObjectA;
					GO::processCollision(gameObjectB, collisionData);
				}
			}
		}
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
		default:
			Log::error("Physics::NextDebugmode", "Unimplemented or Invalid Mode");
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

		for(CollisionShape* shape : sShapes)
		{
			if(shape)
			{
				//shape->cleanup();
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

	namespace CollisionShapes
	{
		Sphere* createSphere(float radius)
		{
			return new Sphere(radius);
		}

		Box* createBox(Vec3 halfExtent)
		{
			return new Box(halfExtent);
		}

		Capsule* createCapsule(float radius, float height)
		{
			return new Capsule(radius, height);
		}

		Plane* createPlane(Vec3 normal, float margin)
		{
			return new Plane(normal, margin);
		}

		Cylinder* createCylinder(Vec3 halfExtent, Vec3 axis)
		{
			return new Cylinder(halfExtent, axis);
		}

		CollisionMesh* createCollisionMesh(CModel* model, bool isTriMesh)
		{
			return new CollisionMesh(model, isTriMesh);
		}
	}

	void generateBindings()
	{
		Sqrat::Class<CollisionShape> collisionShape(ScriptEngine::getVM());
		Sqrat::RootTable().Bind("CollisionShape", collisionShape);
		
		Sqrat::RootTable().Bind("Sphere", Sqrat::DerivedClass<Sphere, CollisionShape>()
								.Ctor<float>());
		Sqrat::RootTable().Bind("Box", Sqrat::DerivedClass<Box, CollisionShape>()
								.Ctor<Vec3>());
		Sqrat::RootTable().Bind("Capsule", Sqrat::DerivedClass<Capsule, CollisionShape>()
								.Ctor<float, float>());
		Sqrat::RootTable().Bind("Plane", Sqrat::DerivedClass<Plane, CollisionShape>()
								.Ctor<Vec3, float>());
		Sqrat::RootTable().Bind("Cone", Sqrat::DerivedClass<Cone, CollisionShape>()
								.Ctor<float, float>());
		Sqrat::RootTable().Bind("Cylinder", Sqrat::DerivedClass<Cylinder, CollisionShape>()
								.Ctor<Vec3, Vec3>());
		Sqrat::RootTable().Bind("CollisionMesh",
								Sqrat::DerivedClass<CollisionMesh, CollisionShape>()
								.Ctor<CModel*, bool>());

		Sqrat::RootTable().Bind("CollisionData", Sqrat::Class<CollisionData>()
								.Var("collidingObj", &CollisionData::collidingObj)
								.Var("worldPosA",    &CollisionData::worldPosA)
								.Var("worldPosB",    &CollisionData::worldPosB)
								.Var("normal",       &CollisionData::normal));

		Sqrat::RootTable().Bind("CollisionShapes", Sqrat::Table(ScriptEngine::getVM())
								.Func("createSphere",   &CollisionShapes::createSphere)
								.Func("createBox",      &CollisionShapes::createBox)
								.Func("createCapsule",  &CollisionShapes::createCapsule)
								.Func("createPlane",    &CollisionShapes::createPlane)
								.Func("createCylinder", &CollisionShapes::createCylinder)
								.Func("createMesh",     &CollisionShapes::createCollisionMesh));
		
	}

	namespace RigidBody
	{
		CRigidBody create(GameObject*     gameObject,
						  CollisionShape* shape,
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
			body->setUserPointer(gameObject);
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

		void applyForce(CRigidBody body, Vec3 force, Vec3 relPos)
		{
			sRigidBodies[body]->applyForce(Utils::toBullet(force),
										   Utils::toBullet(relPos));
		}

		void generateBindings()
		{
			Sqrat::RootTable().Bind("RigidBody", Sqrat::Table(ScriptEngine::getVM())
									.Func("applyForce", &applyForce)
									.Func("remove", &remove)
									.Func("setMass", &setMass)
									.Func("setKinematic", &setKinematic));
		}
	}
}
