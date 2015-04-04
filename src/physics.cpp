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

	void draw()
	{
		CCamera* camera = Renderer::Camera::getActiveCamera();
		if(sEnableDebugDraw && camera != NULL)
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

	void syncWithRenderer()
	{
		for(int i = 0; i < (int)sRigidBodies.size(); i++)
		{
			btRigidBody* body = sRigidBodies[i];
			intptr_t tempNode = (intptr_t)body->getUserPointer();
			Node node = (Node)tempNode;
			GameObject* gameObject = SceneManager::find(node);
			CTransform* transform = GO::getTransform(gameObject);
			if(transform->isModified)
			{
				Mat4 transformMat = transform->transMat;
				RigidBody::setTransform(i, transformMat);
				RigidBody::setActivation(i, true);
			}
		}
	}

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
			CTransform* transform = GO::getTransform(gameObject);
			setTransform(rbHandle, transform->transMat);
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
			if(mass != 0) shape->calculateLocalInertia(mass, inertia);
			sRigidBodies[body]->setMassProps(mass, inertia);
		}

		void applyForce(CRigidBody body, Vec3 force, Vec3 relPos)
		{
			sRigidBodies[body]->applyForce(Utils::toBullet(force),
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
			rc = engine->RegisterGlobalFunction("void setMass(int, const float)",
													asFUNCTION(setMass),
													asCALL_CDECL);
			PA_ASSERT(rc >= 0);
			rc = engine->RegisterGlobalFunction("void applyForce(int, const Vec3, const Vec3 = Vec3(0.f))",
													asFUNCTION(applyForce),
													asCALL_CDECL);
			PA_ASSERT(rc >= 0);
			engine->SetDefaultNamespace("");
		}
	}
}
