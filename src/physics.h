#ifndef PHYSICS_H
#define PHYSICS_H

#define GLM_FORCE_RADIANS
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/bullet/BulletCollision/CollisionShapes/btShapeHull.h"
// #include "../include/bullet/btGImpactConvexDecompositionShape.h"
#include "../include/bullet/btBulletDynamicsCommon.h"

#include "physicsdebugdrawer.h"
#include "utilities.h"
#include "model.h"

class  btDiscreteDynamicsWorld;
class  btMotionState;
class  CollisionShape;
struct CCamera;
struct CTransform;

typedef btDiscreteDynamicsWorld PhysicsWorld;
typedef int32_t                 CRigidBody;

namespace Physics
{
	enum class DBG_Mode : int
	{
		NONE             = btIDebugDraw::DBG_NoDebug,
		WIREFRAME        = btIDebugDraw::DBG_DrawWireframe,
		AABB             = btIDebugDraw::DBG_DrawAabb,
		CONTACTS         = btIDebugDraw::DBG_DrawContactPoints,
		FEATURES_TEXT    = btIDebugDraw::DBG_DrawFeaturesText,
		NO_DEACTIVATION  = btIDebugDraw::DBG_NoDeactivation,
		NO_HELP_TEXT     = btIDebugDraw::DBG_NoHelpText,
		TEXT             = btIDebugDraw::DBG_DrawText,
		PROFIEL_TIMING   = btIDebugDraw::DBG_ProfileTimings,
		ENABLE_SAT_COMP  = btIDebugDraw::DBG_EnableSatComparison,
		DISABLE_LCP      = btIDebugDraw::DBG_DisableBulletLCP,
		ENABLE_CCD       = btIDebugDraw::DBG_EnableCCD,
		CONSTRAINTS      = btIDebugDraw::DBG_DrawConstraints,
		CONSTRAINT_LIMIT = btIDebugDraw::DBG_DrawConstraintLimits,
        NORMALS          = btIDebugDraw::DBG_DrawNormals,
		MAX              = btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE
	};
	
	void initialize(Vec3 gravity);
	void update(float deltaTime);
    void draw(CTransform* viewerTransform, CCamera* viewerCamera);
	void cleanup();
	void setGravity(Vec3 gravity);
    void enableDebugDraw(bool enable);
	void nextDebugMode();
	void setDebugMode(DBG_Mode debugMode);
	void addCollisionShape(CollisionShape* shape);
	void generateBindings();
	
	const Vec3 getGravity();


	namespace RigidBody
	{
		CRigidBody create(CollisionShape* shape,
						  btMotionState*  motionState,
						  float           mass,
						  float           restitution);
		void setTransform(CRigidBody body, Vec3  position, Quat  rotation);
		void getTransform(CRigidBody body, Vec3* position, Quat* rotation);
		void setTransform(CRigidBody body, Mat4 transformMat);
		void applyForce(CRigidBody body, Vec3 force, Vec3 relPos = Vec3(0.f));
		void remove(CRigidBody body);
		void setActivation(CRigidBody body, bool activation);
		void setMass(CRigidBody body, const float mass);
		void setKinematic(CRigidBody body, bool kinematic);
		void generateBindings();
	}
}


class CollisionShape
{
protected:
	btCollisionShape* mShape;
	
public:
	btCollisionShape* getCollisionShape()
	{
		return mShape;
	}

	CollisionShape() : mShape(NULL) {}
	
	virtual void initialize() {}
	
	~CollisionShape()
	{
		if(mShape)
			delete mShape;
	}
};

class Sphere : public CollisionShape
{
	float mRadius;
public:
	Sphere(float radius)
	{
		mRadius = radius;
		initialize();
	}

	void initialize()
	{
		mShape = new btSphereShape(mRadius);
		Physics::addCollisionShape(this);
	}
};

class Box : public CollisionShape
{
	Vec3 mHalfExtent;
public:
	
	Box(Vec3 halfExtent)
	{
		mHalfExtent = halfExtent;
		initialize();
	}

	void initialize()
	{
		mShape = new btBoxShape(Utils::toBullet(mHalfExtent));
		Physics::addCollisionShape(this);
	}
	
};

class Capsule : public CollisionShape
{
	float mRadius;
	float mHeight;
public:

	Capsule(float radius, float height)
	{
		mRadius = radius;
		mHeight = height;
		initialize();
	}

	void initialize()
	{
		mShape = new btCapsuleShape(btScalar(mRadius), btScalar(mHeight));
		Physics::addCollisionShape(this);
	}
};

class Plane : public CollisionShape
{
	float mMargin;
	Vec3  mNormal;

public:

	Plane(Vec3 normal, float margin)
	{
		mNormal = normal;
		mMargin = margin;
		initialize();
	}

	void initialize()
	{
		mShape = new btStaticPlaneShape(Utils::toBullet(mNormal), btScalar(mMargin));
		Physics::addCollisionShape(this);
	}
};

class Cone : public CollisionShape
{
	float mRadius;
	float mHeight;
public:

	Cone(float radius, float height)
	{
		mRadius = radius;
		mHeight = height;
		initialize();
	}

	void initialize()
	{
		mShape = new btConeShape(btScalar(mRadius), btScalar(mHeight));
		Physics::addCollisionShape(this);
	}
};

class Cylinder : public CollisionShape
{
	Vec3 mHalfExtent;
	Vec3 mAxis;
public:

	Cylinder(Vec3 halfExtent, Vec3 axis)
	{
		mHalfExtent = halfExtent;
		mAxis       = axis;
		initialize();
	}

	void initialize()
	{
		if(mAxis == Vec3(0, 1, 0))
			mShape = new btCylinderShape(Utils::toBullet(mHalfExtent));
		else if(mAxis == Vec3(1, 0, 0))
			mShape = new btCylinderShapeX(Utils::toBullet(mHalfExtent));
		else if(mAxis == Vec3(0, 0, 1))
			mShape = new btCylinderShapeZ(Utils::toBullet(mHalfExtent));

		Physics::addCollisionShape(this);
	}
};

class CollisionMesh : public CollisionShape
{
	CModel mModel;
	bool   mTriMesh;
public:

	CollisionMesh(CModel& model, bool isTriMesh)
	{
		mModel   = model;
		mTriMesh = isTriMesh;
		initialize();
	}

	void initialize()
	{
		if(mModel.node != 0)
		{
			float* vertices = Renderer::Model::getVertices(&mModel);
			int vertexCount = Renderer::Model::getVertexCount(&mModel);
			
			btTriangleMesh *triMesh = new btTriangleMesh();

			for(int i = 0; i < vertexCount; i += 9)
			{
				btVector3 v1(vertices[i],     vertices[i + 1], vertices[i + 2]);
				btVector3 v2(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
				btVector3 v3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

				triMesh->addTriangle(v1, v2, v3);
			}

			if(this->mTriMesh)
			{
				mShape = new btBvhTriangleMeshShape(triMesh, false);
				// mShape = new btGImpactConvexDecompositionShape(triMesh, btVector3(1, 1, 1), btScalar(0.1f), true);
			}
			else
			{
				btConvexShape *tempConShape = new btConvexTriangleMeshShape(triMesh);
				btShapeHull *hull = new btShapeHull(tempConShape);
				btScalar margin = tempConShape->getMargin();
				hull->buildHull(margin);
				tempConShape->setUserPointer(hull);
				mShape = new btConvexHullShape(&(hull->getVertexPointer()->getX()),
											   hull->numVertices());
				delete tempConShape;
			}

			Physics::addCollisionShape(this);
		}
		else
			Log::warning("Model provided for collision mesh is Invalid");
	}
};

#endif
