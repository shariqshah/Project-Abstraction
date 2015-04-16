#include "boundingvolumes.h"
#include "transform.h"

namespace BoundingVolume
{
	int isIntersecting(Frustum* frustum, BoundingBox* boundingBox, CTransform* bboxTransform)
	{
		int pointsIntersecting = 0;
		for(Vec3& point : boundingBox->points)
		{
			Vec3 transformedPoint = point + bboxTransform->position;
			transformedPoint *= bboxTransform->scale;
			if(isIntersecting(frustum, transformedPoint))
				pointsIntersecting++;
		}
		int intersectionType = IT_OUTSIDE;
		if(pointsIntersecting > 0)
		{
			intersectionType = IT_INTERSECT;
			if(pointsIntersecting == 8)
				intersectionType = IT_INSIDE;
		}
		return intersectionType;
	}

	bool isIntersecting(Frustum* frustum, const Vec3& point)
	{
		for(int i = 0; i < 6; i++)
		{
			if((frustum->planes[i].x * point.x +
				frustum->planes[i].y * point.y +
				frustum->planes[i].z * point.z +
				frustum->planes[i].w) < 0 )
				return false;
		}
		return true;
	}
}
