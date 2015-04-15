#include "collisionshapes.h"
#include "physics.h"
#include "passert.h"
#include "utilities.h"
#include "log.h"
#include "model.h"
#include "geometry.h"

#include "../include/bullet/btBulletDynamicsCommon.h"
#include "../include/bullet/BulletCollision/CollisionShapes/btShapeHull.h"
// #include "../include/bullet/btGImpactConvexDecompositionShape.h"

btCollisionShape* CollisionShape::getCollisionShape()
{
	return mShape;
}

CollisionShape::CollisionShape() : mShape(NULL) {}	
void CollisionShape::initialize() {}
	
CollisionShape:: ~CollisionShape()
{
	if(mShape)
		delete mShape;
}

bool CollisionShape::isValid()
{
	return valid;
}

Sphere::Sphere(float radius)
{
	mRadius = radius;
	initialize();
}

void Sphere::initialize()
{
	mShape = new btSphereShape(mRadius);
	Physics::addCollisionShape(this);
}
	
Box::Box(Vec3 halfExtent)
{
	mHalfExtent = halfExtent;
	initialize();
}

void Box::initialize()
{
	mShape = new btBoxShape(Utils::toBullet(mHalfExtent));
	Physics::addCollisionShape(this);
}

Capsule::Capsule(float radius, float height)
{
	mRadius = radius;
	mHeight = height;
	initialize();
}

void Capsule::initialize()
{
	mShape = new btCapsuleShape(btScalar(mRadius), btScalar(mHeight));
	Physics::addCollisionShape(this);
}

Plane::Plane(Vec3 normal, float margin)
{
	mNormal = normal;
	mMargin = margin;
	initialize();
}

void Plane::initialize()
{
	mShape = new btStaticPlaneShape(Utils::toBullet(mNormal), btScalar(mMargin));
	Physics::addCollisionShape(this);
}

Cone::Cone(float radius, float height)
{
	mRadius = radius;
	mHeight = height;
	initialize();
}

void Cone::initialize()
{
	mShape = new btConeShape(btScalar(mRadius), btScalar(mHeight));
	Physics::addCollisionShape(this);
}

Cylinder::Cylinder(Vec3 halfExtent, Vec3 axis)
{
	mHalfExtent = halfExtent;
	mAxis       = axis;
	initialize();
}

void Cylinder::initialize()
{
	if(mAxis == Vec3(0, 1, 0))
		mShape = new btCylinderShape(Utils::toBullet(mHalfExtent));
	else if(mAxis == Vec3(1, 0, 0))
		mShape = new btCylinderShapeX(Utils::toBullet(mHalfExtent));
	else if(mAxis == Vec3(0, 0, 1))
		mShape = new btCylinderShapeZ(Utils::toBullet(mHalfExtent));

	Physics::addCollisionShape(this);
}

CollisionMesh::CollisionMesh(const char* filename, bool isTriMesh)
{
	mTriMesh      = isTriMesh;
	geometryIndex = Geometry::create(filename);
	if(geometryIndex == -1)
		valid = false;
	else
		initialize();
}

void CollisionMesh::initialize()
{
	const std::vector<Vec3>*         vertices = Geometry::getVertices(geometryIndex);
	const std::vector<unsigned int>* indices  = Geometry::getIndices(geometryIndex);
	if(vertices && vertices->size() > 3)
	{
		btTriangleMesh *triMesh = new btTriangleMesh();
		if(indices->size() == 0)
		{
			for(int i = 0; i < (int)vertices->size(); i += 3)
			{
				Vec3 vert1 = vertices->at(i);
				Vec3 vert2 = vertices->at(i + 1);
				Vec3 vert3 = vertices->at(i + 2);
				btVector3 v1(Utils::toBullet(vert1));
				btVector3 v2(Utils::toBullet(vert2));
				btVector3 v3(Utils::toBullet(vert3));
				triMesh->addTriangle(v1, v2, v3);
			}
		}
		else
		{
			for(int i = 0; i < (int)indices->size(); i += 3)
			{
				unsigned int index = indices->at(i);
				Vec3 vert1 = vertices->at(index);
				Vec3 vert2 = vertices->at(index + 1);
				Vec3 vert3 = vertices->at(index + 2);
				btVector3 v1(Utils::toBullet(vert1));
				btVector3 v2(Utils::toBullet(vert2));
				btVector3 v3(Utils::toBullet(vert3));
				triMesh->addTriangle(v1, v2, v3);
			}
		}
		if(mTriMesh)
		{
			mShape = new btBvhTriangleMeshShape(triMesh, false);
		}
		else
		{
			btConvexShape *tempConShape = new btConvexTriangleMeshShape(triMesh);
			btShapeHull *hull = new btShapeHull(tempConShape);
			btScalar margin = tempConShape->getMargin();
			hull->buildHull(margin);
			tempConShape->setUserPointer(hull);
			mShape = new btConvexHullShape(&(hull->getVertexPointer()->getX()), hull->numVertices());
			delete tempConShape;
		}
		Physics::addCollisionShape(this);
		Geometry::remove(geometryIndex);
	}
	else
	{
		Log::warning("CollisionMesh::Initialize : Model provided for collision mesh is Invalid");
	}
}
