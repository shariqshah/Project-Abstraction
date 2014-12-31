#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>

#include "mathdefs.h"

class btVector3;
class btQuaternion;

namespace Utils
{   
    const std::string toString(const Vec3  vector);
    const std::string toString(const Vec3* vector);
    const std::string toString(const Vec4  vector);
	const std::string toString(const Quat  quat);
    const std::string toString(const btVector3 vector);

    btVector3    toBullet(const Vec3 glmVec3);
    btQuaternion toBullet(const Quat glmQuat);

    Vec3 toGlm(const btVector3 bulletVec3);
    Quat toGlm(const btQuaternion bulletQuat);

	std::string loadFileIntoString(const char* name);
	char*       loadFileIntoCString(const char* name,
									bool addNull = true);
	
}

#endif
