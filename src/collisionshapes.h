#ifndef _collisionshapes_H
#define _collisionshapes_H

#include "mathdefs.h"

class  btCollisionShape;
struct CModel;
class CollisionShape
{
protected:
	btCollisionShape* mShape;
	
public:
	btCollisionShape* getCollisionShape();
	CollisionShape();
	~CollisionShape();
	virtual void initialize();
};

class Sphere : public CollisionShape
{
	float mRadius;
public:
	Sphere(float radius);
	void initialize();
};

class Box : public CollisionShape
{
	Vec3 mHalfExtent;
public:
	Box(Vec3 halfExtent);
	void initialize();	
};

class Capsule : public CollisionShape
{
	float mRadius;
	float mHeight;
public:
	Capsule(float radius, float height);
	void initialize();
};

class Plane : public CollisionShape
{
	float mMargin;
	Vec3  mNormal;
public:
	Plane(Vec3 normal, float margin);
	void initialize();
};

class Cone : public CollisionShape
{
	float mRadius;
	float mHeight;
public:
	Cone(float radius, float height);
	void initialize();
};

class Cylinder : public CollisionShape
{
	Vec3 mHalfExtent;
	Vec3 mAxis;
public:
	Cylinder(Vec3 halfExtent, Vec3 axis);
	void initialize();
};

class CollisionMesh : public CollisionShape
{
	const CModel* mModel;
	bool           mTriMesh;
public:
	CollisionMesh(const CModel* model, bool isTriMesh = false);
	void initialize();
};

#endif
