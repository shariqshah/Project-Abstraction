#ifndef boundingvolumes_H
#define boundingvolumes_H

#include "mathdefs.h"

struct CTransform;

struct BoundingBox
{
	Vec3 min;
	Vec3 max;
	Vec3 size;
	Vec3 center;
	Vec3 points[8];
};

struct BoundingSphere
{
	Vec3 center;
	float radius;
};

struct Frustum
{
	enum FrustumPlanes
	{
		LEFT = 0,
		RIGHT,
		BOTTOM,
		TOP,
		NEAR,
		FAR,
	};
	Vec4 planes[6];
};

enum IntersectionType
{
	IT_OUTSIDE = 0,
	IT_INTERSECT,
	IT_INSIDE,
};

namespace BoundingVolume
{
	int  isIntersecting(Frustum* frustum, BoundingBox* boundingBox, CTransform* transform);
	int  isIntersecting(Frustum* frustum, BoundingSphere* boundingSphere, CTransform* transform);
	bool isIntersecting(Frustum* frustum, const Vec3& point);
}

#endif
