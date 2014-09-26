#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>

#define GLM_FORCE_RADIANS
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/quaternion.hpp"

namespace Utils
{   
    std::string toString(glm::vec3 vector);
    std::string toString(const glm::vec3 *vector);
    std::string toString(glm::vec4 vector);
	std::string toString(glm::quat quat);
    // std::string toString(btVector3 vector);
    // btVector3 glmToBullet(glm::vec3 glmVec3);
    // btVector3 glmToBullet(const glm::vec3 *glmVec3);
    // btQuaternion glmToBullet(glm::quat glmQuat);
    // glm::vec3 bulletToGlm(btVector3 bulletVec3);
    // glm::quat bulletToGlm(btQuaternion bulletQuat)
}

#endif // UTILITIES_H
