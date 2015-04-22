#ifndef geometry_H
#define geometry_H

#include <string>
#include <vector>

#include "mathdefs.h"

struct CTransform;
struct Frustum;

enum CullingMode
{
	CM_NONE = 0,
	CM_BOX,
	CM_SPHERE
};

namespace Geometry
{
	int                          create(const char* filename);
	int                          find(const char* filename);
	void                         initialize(const char* path);
	void                         cleanup();
	void                         remove(int index);
	void                         setCullingMode(CullingMode mode);
	int                          render(int index, Frustum* frustum, CTransform* transform);
	unsigned int                 getVAO(int index);
	const std::string            getName(int index);
	const std::vector<Vec3>*     getVertices(int index);
	const std::vector<Vec3>*     getNormals(int index);
	const std::vector<Vec3>*     getVertexColors(int index);
	const std::vector<Vec2>*     getUVs(int index);
	const std::vector<uint32_t>* getIndices(int index);
}

#endif

