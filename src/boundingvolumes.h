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

struct Frustum
{
	enum FrustumPlanes
	{
		RIGHT = 0,
		LEFT,
		TOP,
		BOTTOM,
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
	int  isIntersecting(Frustum* frustum, BoundingBox* boundingBox, CTransform* bboxTransform);
	bool isIntersecting(Frustum* frustum, const Vec3& point);
}

#endif
