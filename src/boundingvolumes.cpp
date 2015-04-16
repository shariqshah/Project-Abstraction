#include "boundingvolumes.h"
#include "transform.h"

namespace BoundingVolume
{
	int isIntersecting(Frustum* frustum, BoundingBox* boundingBox, CTransform* bboxTransform)
	{
		
	}

	bool isIntersecting(Frustum* frustum, CTransform* transform)
	{
		Vec3 position = transform->position;
		for(int i = 0; i < 6; i++)
		{
			if((frustum->planes[i].x * position.x +
				frustum->planes[i].y * position.y +
				frustum->planes[i].z * position.z +
				frustum->planes[i].w) <= 0 )
				return false;
		}
		return true;
	}
}
