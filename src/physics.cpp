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
		btDiscreteDynamicsWorld*             world;
		btBroadphaseInterface*               broadphase;
		btDefaultCollisionConfiguration*     collisionConfiguration;
		btCollisionDispatcher*               dispatcher;
		btSequentialImpulseConstraintSolver* solver;
		std::vector<CollisionShape*>         collisionShapes;
		btVector3                            gravity;				  
		const float  						 fixedStep = 1.f / 60.f;
		DebugDrawer* 						 debugDrawer;		 
		DBG_Mode     						 currentDebugMode;		 
		bool         						 physicsEnabled;		 
		bool         						 debugDrawEnabled;
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
		CCamera* camera = Camera::getActiveCamera();
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

	void cleanup()
	{
		for(CollisionShape* shape : collisionShapes) if(shape) delete shape;
		delete world;
		delete solver;
		delete dispatcher;
		delete collisionConfiguration;
		delete broadphase;
		delete debugDrawer;
	}

	btDiscreteDynamicsWorld* getWorld()
	{
		return world;
	}

	void setGravity(Vec3 newGravity)
	{
		gravity = Utils::toBullet(newGravity);
		world->setGravity(gravity);
	}

	Vec3 getGravity()
	{
		return Utils::toGlm(gravity);
	}

	void addCollisionShape(CollisionShape* shape)
	{
		intptr_t index = (intptr_t)collisionShapes.size();
		shape->getCollisionShape()->setUserPointer((void*)index);
		collisionShapes.push_back(shape);
	}

	CollisionShape* getCollisionShapeAtIndex(int index)
	{
		CollisionShape* shape = NULL;
		if(index > -1 && index < collisionShapes.size())
			shape = collisionShapes[index];

		return shape;
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

		CollisionShape* createCollisionMesh(const std::string& filename, bool isTriMesh)
		{
			return new CollisionMesh(filename.c_str(), isTriMesh);
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
}
