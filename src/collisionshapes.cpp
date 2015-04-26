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
	return shape;
}

int CollisionShape::getType()
{
	return CS_INVALID;
}

CollisionShape::CollisionShape() : shape(NULL) {}	
void CollisionShape::initialize() {}
	
CollisionShape:: ~CollisionShape()
{
	if(shape)
		delete shape;
}

bool CollisionShape::isValid()
{
	return valid;
}

Sphere::Sphere(float radius)
{
	this->radius = radius;
	initialize();
}

int Sphere::getType()
{
	return CS_SPHERE;
}

void Sphere::initialize()
{
	shape = new btSphereShape(radius);
	Physics::addCollisionShape(this);
}
	
Box::Box(Vec3 halfExtent)
{
	this->halfExtent = halfExtent;
	initialize();
}

int Box::getType()
{
	return CS_BOX;
}


void Box::initialize()
{
	shape = new btBoxShape(Utils::toBullet(halfExtent));
	Physics::addCollisionShape(this);
}

Capsule::Capsule(float radius, float height)
{
	this->radius = radius;
	this->height = height;
	initialize();
}

int Capsule::getType()
{
	return CS_CAPSULE;
}

void Capsule::initialize()
{
	shape = new btCapsuleShape(btScalar(radius), btScalar(height));
	Physics::addCollisionShape(this);
}

Plane::Plane(Vec3 normal, float margin)
{
	this->normal = normal;
	this->margin = margin;
	initialize();
}

void Plane::initialize()
{
	shape = new btStaticPlaneShape(Utils::toBullet(normal), btScalar(margin));
	Physics::addCollisionShape(this);
}

int Plane::getType()
{
	return CS_PLANE;
}

Cone::Cone(float radius, float height)
{
	this->radius = radius;
	this->height = height;
	initialize();
}

int Cone::getType()
{
	return CS_CONE;
}

void Cone::initialize()
{
	shape = new btConeShape(btScalar(radius), btScalar(height));
	Physics::addCollisionShape(this);
}

Cylinder::Cylinder(Vec3 halfExtent, Vec3 axis)
{
	this->halfExtent = halfExtent;
	this->axis       = axis;
	initialize();
}

int Cylinder::getType()
{
	return CS_CYLINDER;
}

void Cylinder::initialize()
{
	if(axis == Vec3(0, 1, 0))
	{
		shape = new btCylinderShape(Utils::toBullet(halfExtent));
	}
	else if(axis == Vec3(1, 0, 0))
	{
		shape = new btCylinderShapeX(Utils::toBullet(halfExtent));
	}
	else if(axis == Vec3(0, 0, 1))
	{
		shape = new btCylinderShapeZ(Utils::toBullet(halfExtent));
	}
	else
	{
		Log::error("Cylinder::initialize", "Invalid axis, reverting to default(0, 1, 0)");
		axis = Vec3(0, 1, 0);
		shape = new btCylinderShape(Utils::toBullet(halfExtent));
	}
	Physics::addCollisionShape(this);
}

int CollisionMesh::getType()
{
	return triMesh ? CS_CONVEX_MESH : CS_CONCAVE_MESH;
}

CollisionMesh::CollisionMesh(const char* filename, bool isTriMesh)
{
	triMesh       = isTriMesh;
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
		if(triMesh)
		{
			shape = new btBvhTriangleMeshShape(triMesh, false);
		}
		else
		{
			btConvexShape *tempConShape = new btConvexTriangleMeshShape(triMesh);
			btShapeHull *hull = new btShapeHull(tempConShape);
			btScalar margin = tempConShape->getMargin();
			hull->buildHull(margin);
			tempConShape->setUserPointer(hull);
			shape = new btConvexHullShape(&(hull->getVertexPointer()->getX()), hull->numVertices());
			delete tempConShape;
		}
		Physics::addCollisionShape(this);
		Geometry::remove(geometryIndex);
	}
	else
	{
		Log::warning("CollisionMesh::Initialize : Model provided for collision mesh is Invalid");
		valid = false;
	}
}
