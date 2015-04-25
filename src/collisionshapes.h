#ifndef _collisionshapes_H
#define _collisionshapes_H

#include "mathdefs.h"

class btCollisionShape;

enum CollisionShapeType
{
	CS_SPHERE = 0,
	CS_BOX,
	CS_CAPSULE,
	CS_PLANE,
	CS_CONE,
	CS_CYLINDER,
	CS_CONCAVE_MESH,
	CS_CONVEX_MESH,
	CS_INVALID
};

class CollisionShape
{
protected:
	btCollisionShape* shape;
	bool              valid = true;
public:
	btCollisionShape* getCollisionShape();
	CollisionShape();
	~CollisionShape();
	virtual void initialize();
	virtual int  getType();
	bool isValid();
};

class Sphere : public CollisionShape
{
public:
	float radius;
	Sphere(float radius = 1.f);
	void initialize();
	virtual int getType();
};

class Box : public CollisionShape
{
public:
	Vec3 halfExtent;
	Box(Vec3 halfExtent = Vec3(0.5f));
	void initialize();
	virtual int getType();
};

class Capsule : public CollisionShape
{
public:
	float radius;
	float height;
	Capsule(float radius = 1.f, float height = 2.f);
	void initialize();
	virtual int getType();
};

class Plane : public CollisionShape
{
public:
	float margin;
	Vec3  normal;
	Plane(Vec3 normal = Vec3(0, 1, 0), float margin = 0.001f);
	void initialize();
	virtual int getType();
};

class Cone : public CollisionShape
{
public:
	float radius;
	float height;
	Cone(float radius = 1.f, float height = 2.f);
	void initialize();
	virtual int getType();
};

class Cylinder : public CollisionShape
{
public:
	Vec3 halfExtent;
	Vec3 axis;
	Cylinder(Vec3 halfExtent = Vec3(0.5f), Vec3 axis = Vec3(0, 1, 0));
	void initialize();
	virtual int getType();
};

class CollisionMesh : public CollisionShape
{
public:
	int  geometryIndex;
	bool triMesh;
	CollisionMesh(const char* filename = "default.pamesh", bool isTriMesh = false);
	void initialize();
	virtual int getType();
};

#endif
