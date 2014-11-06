#ifndef PHYSICS_H
#define PHYSICS_H

#include <memory>

#define GLM_FORCE_RADIANS
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/bullet/BulletCollision/CollisionShapes/btShapeHull.h"
#include "../include/bullet/btGImpactConvexDecompositionShape.h"

#include "physicsdebugdrawer.h"
#include "utilities.h"
#include "transform.h"
#include "model.h"
#include "scenemanager.h"

typedef btDiscreteDynamicsWorld PhysicsWorld;
typedef uint32_t                RBHandle;

class MotionState;
class CollisionShape;

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
	
	bool initialize(glm::vec3 gravity);

	RBHandle createRigidBody(CollisionShape* shape,
							 MotionState*    motionState,
							 float           mass,
							 float           restitution);

	void update(float deltaTime);
    void draw();
	void cleanup();
	void setGravity(glm::vec3 gravity);
    void enableDebugDraw(bool enable);
	void nextDebugMode();
	void setDebugMode(DBG_Mode debugMode);
	
	void setTransform(RBHandle body, glm::vec3  position, glm::quat  rotation);
	void getTransform(RBHandle body, glm::vec3* position, glm::quat* rotation);
	void setTransform(RBHandle body, glm::mat4 transformMat);
	void applyForce(RBHandle body, glm::vec3 force, glm::vec3 relPos);
	void removeRigidBody(RBHandle body);
	void setActivation(RBHandle body, bool activation);
	void setMass(RBHandle body, const float mass);
	void setKinematic(RBHandle body, bool kinematic);
	void addCollisionShape(CollisionShape* shape);

	const glm::vec3 getGravity();
}

class MotionState : public btMotionState
{
	std::weak_ptr<Transform> mTransformPtr;

public:
	MotionState(std::shared_ptr<Transform> transform)
	{
		mTransformPtr = transform;
	}

	virtual ~MotionState() {}

	virtual void getWorldTransform(btTransform& worldTrans) const
	{
		if(auto transform = mTransformPtr.lock())
		{
			glm::mat4 transformMat = transform->getTransformMat();
			worldTrans.setFromOpenGLMatrix(glm::value_ptr(transformMat));
		}
	}

	virtual void setWorldTransform(const btTransform& worldTrans)
	{
		if(auto transform = mTransformPtr.lock())
		{
			// update transform and reset renderer's transformation
			// flag, indicating that transform has been updated by bullet
			transform->setPosition(Utils::toGlm(worldTrans.getOrigin()));
			transform->setRotation(Utils::toGlm(worldTrans.getRotation()));
			transform->resetTransformFlag();
		}
	}
   
};

class CollisionShape
{
protected:
	btCollisionShape *mShape;
	
public:
	btCollisionShape* getCollisionShape()
	{
		return mShape;
	}

	CollisionShape() : mShape(NULL) {}

	void cleanup() { if(mShape) delete mShape; }
	
	virtual void initialize() = 0;
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
	glm::vec3 mHalfExtent;
public:
	
	Box(glm::vec3 halfExtent)
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
	float     mMargin;
	glm::vec3 mNormal;

public:

	Plane(glm::vec3 normal, float margin)
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
	glm::vec3 mHalfExtent;
	glm::vec3 mAxis;
public:

	Cylinder(glm::vec3 halfExtent, glm::vec3 axis)
	{
		mHalfExtent = halfExtent;
		mAxis       = axis;
		initialize();
	}

	void initialize()
	{
		if(mAxis == glm::vec3(0, 1, 0))
			mShape = new btCylinderShape(Utils::toBullet(mHalfExtent));
		else if(mAxis == glm::vec3(1, 0, 0))
			mShape = new btCylinderShapeX(Utils::toBullet(mHalfExtent));
		else if(mAxis == glm::vec3(0, 0, 1))
			mShape = new btCylinderShapeZ(Utils::toBullet(mHalfExtent));

		Physics::addCollisionShape(this);
	}
};

class CollisionMesh : public CollisionShape
{
	std::weak_ptr<Model> mModel;
	bool mTriMesh;
public:

	CollisionMesh(std::shared_ptr<Model> model, bool isTriMesh)
	{
		mModel   = model;
		mTriMesh = isTriMesh;
		initialize();
	}

	void initialize()
	{
		if(auto model = mModel.lock())
		{
			float* vertices = model->getVertices();
			btTriangleMesh *triMesh = new btTriangleMesh();

			for(int i = 0; i < model->getVertexCount(); i += 9)
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
	}
};


#endif
