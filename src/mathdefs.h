#ifndef mathdefs_H
#define mathdefs_H

#define GLM_FORCE_RADIANS
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/quaternion.hpp"
#include "../include/glm/gtx/transform.hpp"
#include "../include/glm/gtx/quaternion.hpp"

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;
typedef glm::mat4 Mat4;
typedef glm::mat3 Mat3;
typedef glm::quat Quat;

namespace MathTypes
{
	void generateBindings();
}

#endif
