#ifndef _transform_H
#define _transform_H

#define GLM_FORCE_RADIANS
#include "../include/glm/gtx/transform.hpp"
#include "../include/glm/gtx/quaternion.hpp"

#include "componentTypes.h"
#include "renderer.h"

typedef glm::vec3 Vec3;
typedef glm::vec2 Vec2;
typedef glm::mat4 Mat4;
typedef glm::quat Quat;

struct CTransform
{
    Vec3 position = Vec3(0.f);
    Vec3 scale    = Vec3(1.f);
    Vec3 lookAt   = Vec3(0.f, 0.f, -5.f);
    Vec3 up       = Vec3(0.f, 1.f, 0.f);
    Vec3 forward  = Vec3(0.f, 0.f, 1.f);
    Quat rotation = Quat();
	Node node     = 0;
	Mat4 transMat = Mat4();
	bool valid    = true;
};

namespace Transform
{
	static const Vec3 UNIT_X = Vec3(1, 0, 0);
	static const Vec3 UNIT_Y = Vec3(0, 1, 0);
	static const Vec3 UNIT_Z = Vec3(0, 0, 1);
	
	enum Space
    {
        LOCAL = 0,
        WORLD
    };

	void setPosition(CTransform* transform,
					 Vec3        position,
					 bool        updateTransMat = true);
	
    void translate(CTransform* transform,
				   Vec3        offset,
				   Space       transformSpace = Space::WORLD);
	
    void rotate(CTransform* transform,
				Vec3        axis,
				float       angle,
				Space       transformSpace = Space::WORLD);
	
    void setScale(CTransform* transform,
				  Vec3        newScale,
				  bool        updateTransMat = true);

	void setRotation(CTransform* transform,
					 Quat        newRotation,
					 bool        updateTransMat = true);
	
    void setLookAt(CTransform* transform, Vec3 lookAt);
	void setUpVector(CTransform* transform, Vec3 up);
    void setForward(CTransform* transform, Vec3 direction);
	void resetTransformFlag(CTransform* transform);
	void updateTransformMatrix(CTransform* transform);
	void initialize();
}

#endif
