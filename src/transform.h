#ifndef _transform_H
#define _transform_H

#include "mathdefs.h"
#include "componentTypes.h"
#include "renderer.h"
#include "datatypes.h"

struct CTransform
{
    Vec3 position   = Vec3(0.f);
    Vec3 scale      = Vec3(1.f);
    Vec3 lookAt     = Vec3(0.f, 0.f, -5.f);
    Vec3 up         = Vec3(0.f, 1.f, 0.f);
    Vec3 forward    = Vec3(0.f, 0.f, -1.f);
    Quat rotation   = Quat();
	Node node       = 0;
	Mat4 transMat   = Mat4();
	bool valid      = true;
};

namespace Transform
{
	enum Space
    {
        TS_LOCAL = 0,
        TS_WORLD
    };
	static const Vec3 UNIT_X = Vec3(1, 0, 0);
	static const Vec3 UNIT_Y = Vec3(0, 1, 0);
	static const Vec3 UNIT_Z = Vec3(0, 0, 1);
	void setPosition(CTransform* transform,	Vec3 position, bool syncPhysics = true);
	void setScale(CTransform* transform, Vec3 newScale, bool syncPhysics = true);
	void setRotation(CTransform* transform, Quat newRotation, bool syncPhysics = true);
    void translate(CTransform* transform, Vec3 offset, Space transformSpace = Space::TS_WORLD);
    void rotate(CTransform* transform, Vec3 axis, float angle, Space transformSpace = Space::TS_WORLD);
    void setLookAt(CTransform* transform, Vec3 lookAt);
	void setUpVector(CTransform* transform, Vec3 up);
    void setForward(CTransform* transform, Vec3 direction);
	void updateTransformMatrix(CTransform* transform, bool syncPhysics = true);
	void generateBindings();
	void cleanup();
	bool remove(unsigned int transformIndex);
	int  create(Node node);
	CTransform* getTransformAtIndex(int transformIndex);
}

#endif
