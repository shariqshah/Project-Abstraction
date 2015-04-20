#include "boundingvolumes.h"
#include "transform.h"

namespace BoundingVolume
{
	int isIntersecting(Frustum* frustum, BoundingBox* box, CTransform* transform)
	{
		Vec3 min = (box->min + transform->position) * transform->scale;
		Vec3 max = (box->max + transform->position) * transform->scale;
		Vec3 size = max - min;
		Vec3 center = (max + min) / 2.f;
		Vec3 halfExt = size / 2.f;
		for(int i = 0; i < 6; i++)
		{
			glm::vec3 normal(frustum->planes[i]);
			float distance = frustum->planes[i].w;
			float d = glm::dot(normal, center);
			float r = glm::dot(halfExt, glm::abs(normal));
			if(d + r < -distance)
			{
				return IT_OUTSIDE;
			}
		}
		return IT_INSIDE;
	}

	int isIntersecting(Frustum* frustum, BoundingSphere* sphere, CTransform* transform)
	{
		int intersectionType = IT_INSIDE;
		Vec3 center = (sphere->center + transform->position) * transform->scale;	
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

			if(glm::abs(dot) < sphere->radius)
			{
				intersectionType = IT_INTERSECT;
				return intersectionType;
			}
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
