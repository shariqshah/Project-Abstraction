#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>

#define GLM_FORCE_RADIANS
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/quaternion.hpp"
#include "../include/bullet/btBulletDynamicsCommon.h"

namespace Utils
{   
    const std::string toString(glm::vec3 vector);
    const std::string toString(const glm::vec3 *vector);
    const std::string toString(glm::vec4 vector);
	const std::string toString(glm::quat quat);
    const std::string toString(btVector3 vector);

    btVector3    toBullet(glm::vec3 glmVec3);
    btQuaternion toBullet(glm::quat glmQuat);

    glm::vec3    toGlm(btVector3 bulletVec3);
    glm::quat    toGlm(btQuaternion bulletQuat);
}

#endif // UTILITIES_H
