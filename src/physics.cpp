#include "physics.h"
#include "camera.h"
#include "transform.h"
#include "scriptengine.h"
#include "gameobject.h"
#include "utilities.h"
#include "collisionshapes.h"
#include "scenemanager.h"
#include "passert.h"

#include "../include/bullet/btBulletDynamicsCommon.h"

namespace Physics
{
	namespace
	{
		PhysicsWorld*                        world;
		btBroadphaseInterface*               broadphase;
		btDefaultCollisionConfiguration*     collisionConfiguration;
		btCollisionDispatcher*               dispatcher;
		btSequentialImpulseConstraintSolver* solver;

		std::vector<CRigidBody>      freeList;
		std::vector<btRigidBody*>    rigidBodies;
		std::vector<CollisionShape*> collisionShapes;

		bool          physicsEnabled;
		bool          debugDrawEnabled;
		btVector3     gravity;
		const float   fixedStep = 1.f / 60.f;
		DebugDrawer*  debugDrawer;
		DBG_Mode      currentDebugMode;
	}

	void initialize(Vec3 worldGravity)
	{
		gravity = Utils::toBullet(worldGravity);
	    broadphase             = new btDbvtBroadphase();
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher             = new btCollisionDispatcher(collisionConfiguration);
		solver                 = new btSequentialImpulseConstraintSolver;
		world                  = new btDiscreteDynamicsWorld(dispatcher,
															  broadphase,
															  solver,
															  collisionConfiguration);
		world->setGravity(gravity);
		currentDebugMode = DBG_Mode::AABB;
		debugDrawer = new Physics::DebugDrawer();
		debugDrawer->setDebugMode((int)currentDebugMode);
		world->setDebugDrawer(debugDrawer);
		physicsEnabled   = true;
		debugDrawEnabled = false;
	}

	bool isEnabled()
	{
		return physicsEnabled;
	}
	
	bool isDebugDrawerEnabled()
	{
		return debugDrawEnabled;
	}
	
	void enable(bool enable)
	{
		physicsEnabled = enable;
	}
	
	void enableDebugDraw(bool enable)
	{
		debugDrawEnabled = enable;
	}
	
	void update(float deltaTime)
	{
		if(physicsEnabled)
		{
			world->stepSimulation(deltaTime, 1, fixedStep);
			// Process collisions
			int numManifolds = world->getDispatcher()->getNumManifolds();
			for (int i=0;i<numManifolds;i++)
			{
				btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
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

						intptr_t tempA = (intptr_t)obA->getUserPointer();
						intptr_t tempB = (intptr_t)obB->getUserPointer();
						Node nodeA = (Node)tempA;
						Node nodeB = (Node)tempB;
				    
						CollisionData collisionData;
						collisionData.collidingObjNode = nodeB;
						collisionData.normal           = Utils::toGlm(normalOnB);
						collisionData.worldPosA        = Utils::toGlm(ptA);
						collisionData.worldPosB        = Utils::toGlm(ptB);

						GameObject* gameObjectA = SceneManager::find(nodeA);
						GO::processCollision(gameObjectA, &collisionData);
						GameObject* gameObjectB = SceneManager::find(nodeB);
						collisionData.collidingObjNode = nodeA;
						GO::processCollision(gameObjectB, &collisionData);
					}
				}
			}
		}
	}

	void draw()
	{
		CCamera* camera = Renderer::Camera::getActiveCamera();
		if(debugDrawEnabled && camera != NULL)
		{
			GameObject* gameObject = SceneManager::find(camera->node);
			CTransform* transform  = GO::getTransform(gameObject);
			Mat4 camTranMat = transform->transMat;
			Mat4 camProjMat = camera->projMat;

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadMatrixf(glm::value_ptr(glm::inverse(camTranMat)));
		
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadMatrixf(glm::value_ptr(camProjMat));

			glMatrixMode(GL_MODELVIEW);
		
			world->debugDrawWorld();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glPopAttrib();
		}
	}

	void setDebugMode(DBG_Mode debugMode)
	{
		currentDebugMode = debugMode;
		world->getDebugDrawer()->setDebugMode((int)currentDebugMode);
	}

	void nextDebugMode()
	{
		DBG_Mode tempMode;
		
		switch(currentDebugMode)
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
		world->removeRigidBody(body);
		if(body && body->getMotionState()) delete body->getMotionState();
		body == NULL ? Log::message("RB is NULL") : delete body;
    }

	void cleanup()
	{
		//Remove collision objects
		for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject *obj = world->getCollisionObjectArray()[i];
			removeCollisionObject(btRigidBody::upcast(obj));
		}
		for(CollisionShape* shape : collisionShapes) if(shape) delete shape;
		
		delete world;
		delete solver;
		delete dispatcher;
		delete collisionConfiguration;
		delete broadphase;
		delete debugDrawer;
		rigidBodies.clear();
	}

	void setGravity(Vec3 newGravity)
	{
		gravity = Utils::toBullet(newGravity);
		world->setGravity(gravity);
	}

	const Vec3 getGravity()
	{
		return Utils::toGlm(gravity);
	}

	void addCollisionShape(CollisionShape* shape)
	{
		collisionShapes.push_back(shape);
	}

	// For scripts only
	namespace CollisionShapes
	{
		CollisionShape* createSphere(float radius)
		{
			return new Sphere(radius);
		}

		CollisionShape* createBox(Vec3 halfExtent)
		{
			return new Box(halfExtent);
		}

		CollisionShape* createCapsule(float radius, float height)
		{
			return new Capsule(radius, height);
		}

		CollisionShape* createPlane(Vec3 normal, float margin)
		{
			return new Plane(normal, margin);
		}

		CollisionShape* createCylinder(Vec3 halfExtent, Vec3 axis)
		{
			return new Cylinder(halfExtent, axis);
		}

		CollisionShape* createCollisionMesh(CModel* model, bool isTriMesh)
		{
			return new CollisionMesh(model, isTriMesh);
		}
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		int rc = engine->RegisterObjectType("CollisionData",
											sizeof(CollisionData),
											asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("CollisionData",
											"int32 collidingObjNode",
											asOFFSET(CollisionData, collidingObjNode));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("CollisionData",
											"Vec3 worldPosA",
											asOFFSET(CollisionData, worldPosA));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("CollisionData",
											"Vec3 worldPosB",
											asOFFSET(CollisionData, worldPosB));
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("CollisionData",
											"Vec3 normal",
											asOFFSET(CollisionData, normal));
		PA_ASSERT(rc >= 0);

		// CollisionShapes
		rc = engine->RegisterObjectType("CollisionShape",
										sizeof(CollisionShape),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectType("Sphere",
										sizeof(Sphere),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectType("Box",
										sizeof(Box),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectType("Capsule",
										sizeof(Capsule),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectType("Plane",
										sizeof(Plane),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectType("Cone",
										sizeof(Cone),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectType("Cylinder",
										sizeof(Cylinder),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectType("CollisionMesh",
										sizeof(CollisionMesh),
										asOBJ_REF | asOBJ_NOCOUNT);
		PA_ASSERT(rc >= 0);

		// Functions for collision shape creation from scripts
		engine->SetDefaultNamespace("CollisionShapes");
		rc = engine->RegisterGlobalFunction("CollisionShape@ createSphere(float = 1.f)",
											asFUNCTION(CollisionShapes::createSphere),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("CollisionShape@ createBox(Vec3)",
											asFUNCTION(CollisionShapes::createBox),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("CollisionShape@ createCapsule(float, float)",
											asFUNCTION(CollisionShapes::createCapsule),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("CollisionShape@ createPlane(Vec3, Vec3)",
											asFUNCTION(CollisionShapes::createPlane),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("CollisionShape@ createCylinder(Vec3, Vec3)",
											asFUNCTION(CollisionShapes::createCylinder),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("CollisionShape@ createCollisionMesh(const Model@, bool = true)",
											asFUNCTION(CollisionShapes::createCollisionMesh),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");

		engine->SetDefaultNamespace("Physics");
		rc = engine->RegisterGlobalFunction("void setGravity(const Vec3)", asFUNCTION(setGravity), asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
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
	}
}
