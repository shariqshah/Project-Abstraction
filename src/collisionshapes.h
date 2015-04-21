#ifndef _collisionshapes_H
#define _collisionshapes_H

#include "mathdefs.h"

class btCollisionShape;

class CollisionShape
{
protected:
	btCollisionShape* mShape;
	bool              valid = true;
public:
	btCollisionShape* getCollisionShape();
	CollisionShape();
	~CollisionShape();
	virtual void initialize();
	bool isValid();
};

class Sphere : public CollisionShape
{
	float mRadius;
public:
	Sphere(float radius = 1.f);
	void initialize();
};

class Box : public CollisionShape
{
	Vec3 mHalfExtent;
public:
	Box(Vec3 halfExtent = Vec3(0.5f));
	void initialize();	
};

class Capsule : public CollisionShape
{
	float mRadius;
	float mHeight;
public:
	Capsule(float radius = 1.f, float height = 2.f);
	void initialize();
};

class Plane : public CollisionShape
{
	float mMargin;
	Vec3  mNormal;
public:
	Plane(Vec3 normal = Vec3(0, 1, 0), float margin = 0.001f);
	void initialize();
};

class Cone : public CollisionShape
{
	float mRadius;
	float mHeight;
public:
	Cone(float radius = 1.f, float height = 2.f);
	void initialize();
};

class Cylinder : public CollisionShape
{
	Vec3 mHalfExtent;
	Vec3 mAxis;
public:
	Cylinder(Vec3 halfExtent = Vec3(0.5f), Vec3 axis = Vec3(0, 1, 0));
	void initialize();
};

class CollisionMesh : public CollisionShape
{
	int  geometryIndex;
	bool mTriMesh;
public:
	CollisionMesh(const char* filename = "default.pamesh", bool isTriMesh = false);
	void initialize();
};

#endif
