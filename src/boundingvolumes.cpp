#include "boundingvolumes.h"
#include "transform.h"

namespace BoundingVolume
{
	int isIntersecting(Frustum* frustum, BoundingBox* box, CTransform* transform)
	{
		int intersectionType = IT_OUTSIDE;
		// for(Vec3& point : box->points)
		// {
		// 	Vec3 transformedPoint = point + transform->position;
		// 	transformedPoint *= transform->scale;
		// 	if(isIntersecting(frustum, transformedPoint))
		// 		pointsIntersecting++;
		// }
		//int intersectionType = IT_OUTSIDE;

		Vec3 center = transform->position + box->center;
		Vec3 halfExt = box->size / 2.f;
		for (int i = 0; i < 6; i++)
		{
			glm::vec3 normal(frustum->planes[i]);
			float distance = frustum->planes[i].w;

			float d = glm::dot(normal, center);
			float r = glm::dot(halfExt, glm::abs(normal));
			if(d + r < -distance)
			{
				intersectionType = IT_OUTSIDE;
				return IT_OUTSIDE;
			}
			else
			{
				return IT_INTERSECT;
			}
		}

		return IT_INSIDE;
	}

	int isIntersecting(Frustum* frustum, BoundingSphere* sphere, CTransform* transform)
	{
		int intersectionType = IT_INSIDE;
		Vec3 center = sphere->center + transform->position;
		
		for(int i = 0; i < 6; i++)
		{
			Vec3 planeNormal = Vec3(frustum->planes[i]);
			float distance   = frustum->planes[i].w;
			float dot = glm::dot(planeNormal, center) + distance;

			if(dot < -sphere->radius)
			{
				intersectionType = IT_OUTSIDE;
				return intersectionType;
			}
			else
			{
				intersectionType = IT_INTERSECT;
				return intersectionType;
			}
		}

		// if(intersectCount > 0) intersectionType = IT_INTERSECT;

		// for(int i = 0; i < 6; i++)
		// {
		// 	if((frustum->planes[i].x * center.x +
		// 		frustum->planes[i].y * center.y +
		// 		frustum->planes[i].z * center.z +
		// 		frustum->planes[i].w) <= -sphere->radius)
		// 		intersectionType = IT_OUTSIDE; break;
		// }
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
